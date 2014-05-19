<?php

/**
 * @file
 * Collection of forms (including validation/submit handlers) for
 * uploading PDFs and mapping form fields with content type fields.
 *
 */

// See header of fpdm.php for the modifications made to the library.
module_load_include('php', 'pdfrender', 'lib/fpdm/fpdm');

/*
 * This method loads the upload PDF form when the Admin goes to the config page.
 */

function pdfrender_form_router() {
  $output = array();
  $output['upload'] = drupal_get_form('pdfrender_upload_form');
  return $output;
}

/*
 * Form that displays existing PDF forms and allows the user to upload a new one.
 */

function pdfrender_upload_form($form, &$form_state) {
  // Get a set of publicly available PDF forms from filesystem.
  $path = 'public://';
  $existing_forms = file_scan_directory($path, '/.*.pdf$/i', array('min_depth' => 0));

  $items = array();
  foreach ($existing_forms as $pdf) {
    $path = file_create_url($pdf->uri);
    $pdf_link = l($pdf->name, $path);

    // Check to see if a mapping exists for the PDF.
    if ($mapping = _get_mapping($pdf->name)) {
      $mapping_link = l(t('Edit existing mapping'), 'pdfrender/ajax/' . $pdf->name . '.pdf', array('attributes' => array('class' => array('use-ajax'))));
    }
    else {
      $mapping_link = l(t('Create new mapping'), 'pdfrender/ajax/' . $pdf->name . '.pdf', array('attributes' => array('class' => array('use-ajax'))));
    }
    $delete_link = l(t('Delete form'), 'ajax/delete_pdf/' . $pdf->name . '.pdf');

    // Check to see if a content type has been associated with the PDF.
    $selected_content = variable_get('content_' . $pdf->name . '.pdf');
    if (!isset($selected_content)) {
      $selected_content = 'Nothing Selected';
    }

    $items[] = array(
      'pdf_link' => $pdf_link,
      'mapping_link' => $mapping_link,
      'content' => $selected_content,
      'delete_link' => $delete_link,
    );
  }

  $markup = '<table border="1">';
  $markup .= '<tr><td><b>PDF Forms</b></td>' .
    '<td><b>Custom Mapping</b></td>' .
    '<td><b>Content Type</b></td>' .
    '<td></td>' .
    '</tr>';
  foreach ($items as $item) {
    $markup .= '<tr>' .
      '<td>' . $item['pdf_link'] . '</td>' .
      '<td>' . $item['mapping_link'] . '</td>' .
      '<td>' . $item['content'] . '</td>' .
      '<td>' . $item['delete_link'] . '</td>' .
      '</tr>';
  }
  $markup .= '</table>';

  $form['upload'] = array('#title' => 'upload');

  $form['html'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );
  $form['file'] = array(
    '#type' => 'file',
    '#title' => t('Upload a new Form'),
    '#description' => t('Upload a file, allowed extensions: pdf'),
  );

  $content_types = node_type_get_types();
  if (is_array($content_types)) {
    $content_type_options = array('NULL' => '--none--');
    foreach (array_keys($content_types) as $content_field) {
      $content_type_options[$content_field] = $content_field;
    }
  }

  $form['content_type'] = array(
    '#type' => 'select',
    '#title' => t('Select a Content Type'),
    '#options' => $content_type_options,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Upload Form'),
    '#submit' => array('pdfrender_upload_form_submit'),
  );

  $html = '<div id="mappingform"></div';

  $form['test'] = array(
    '#type' => 'markup',
    '#markup' => $html,
  );

  return $form;
}

/*
 * Method for validating a file an Admin uploads as well as the content
 * type they specify.
 *
 */

function pdfrender_upload_form_validate($form, &$form_state) {
  $file = file_save_upload('file', array(
    'file_validate_extensions' => array('pdf'),
  ));

  if (file_exists('public://' . $file->filename)) {
    form_set_error('file', t('Failed to write the uploaded file as another file exists with this name'));
  }

  // Verify if a content type has been selected.
  if (isset($form_state['input']['content_type'])) {
    if ($form_state['input']['content_type'] == 'NULL') {
      form_set_error('content type', t('Please select a content type to associate with the uploaded file'));
      return FALSE;
    }
  }

  // ToDo:  Check to see if file is valid with FPDM lib (editable PDF w/form fields & compatible w/Adobe Acrobat 5.0).
  if ($file) {
    // Move the file, into the Drupal file system
    if ($file = file_move($file, 'public://')) {

      // Save the file for use in the submit handler.
      $form_state['storage']['file'] = $file;
    }
    else {
      form_set_error('file', t('Failed to write the uploaded file to the site\'s file folder.'));
    }
  }
  else {
    form_set_error('file', t('No file was uploaded.'));
  }
}

/*
 * Submit handler for uploading PDF forms.
 */

function pdfrender_upload_form_submit($form, &$form_state) {
  $file = $form_state['storage']['file'];

  // We are done with the file, remove it from storage.
  unset($form_state['storage']['file']);
  // Make the storage of the file permanent
  $file->status = FILE_STATUS_PERMANENT;
  // Save file status.
  file_save($file);

  // Once the file is saved in the filesystem, then save the content type & make association.
  $content_type = $form_state['input']['content_type'];

  variable_set('content_' . $file->filename, $content_type);

  drupal_set_message($form['#form_id'] . ' ' . t('The form has been submitted and the file and content type have been saved, filename: @filename.', array('@filename' => $file->filename)));
}

function pdfrender_custom_mapping_form($form, &$form_state, $filename) {
  $markup = '<h5><a class="cancelmapping">Cancel</a></h5>';

  $form['html'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  if (!empty($filename)) {
    $path = 'public://' . $filename;
    $file = file_create_url($path);
  }

  // Get form fields from the PDF.
  $form_fields_source = _get_pdf_form_fields($file);

  // Get collection of all field labels that are associated with the content type.
  $content_label_drop_down = array('null' => '--none--');
  $selected_content_type = variable_get('content_' . $filename);
  if (isset($selected_content_type)) {
    $content_fields_source = _enumerate_all_content_labels($selected_content_type);
    ksort($content_fields_source);

    foreach (array_keys($content_fields_source) as $content_field) {
      $content_label_drop_down[$content_field] = $content_field;
    }
  }

  $form['container'] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom Mapping'),
    '#tree' => TRUE,
  );

  $form['container']['#filename'] = $filename;

  // Check to see if a mapping already exists for the file.
  // If it does, prepopulate values on the form below.
  $mapping = NULL;
  $mapping = _get_mapping($filename, TRUE);

  foreach ($form_fields_source as $key => $val) {
    if (strlen($key) > 40) {
      $key = substr($key, 0, 40) . '...';
    }

    $form['container'][$key]['formfield'] = array(
      '#type' => 'textfield',
      '#title' => 'Form Field',
      '#value' => $key,
      '#attributes' => array('disabled' => 'disabled'),
      '#prefix' => '<table border= \'0\'><tr><td>',
      '#suffix' => '</td>',
    );

    // See if a content type value exists in the mapping for the field.
    $content_value = NULL;

    if ((!empty($mapping)) && in_array($key, array_keys($mapping))) {
      $content_data = $mapping[$key];
      if (is_array($content_data)) {
        $content_value = array_pop($content_data);
      }
    }

    $form['container'][$key]['projfield'] = array(
      '#type' => 'select',
      '#title' => 'Content Type Field',
      '#options' => $content_label_drop_down,
      '#value' => $content_value,
      '#prefix' => '<td>',
      '#suffix' => '</td></tr></table>',
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Mapping'),
    '#submit' => array('pdfrender_custom_mapping_form_submit'),
  );

  return $form;
}

function pdfrender_custom_mapping_form_validate($form, &$form_state) {

}

/*
 * Add the mapping to the variable table.
 */
function pdfrender_custom_mapping_form_submit($form, &$form_state) {

  $input = $form_state['input']['container'];
  $file = $form['container']['#filename'];
  $custom_mapping = $input;
  variable_set('mapping_' . $file, $custom_mapping);

  drupal_goto('admin/config/content/pdfrender');
}

/*
 * Method that generates a form that presents a user with the ability to
 * enter in a custom mapping for a PDF form.
 */

function pdfrender_mapping($file) {
  $output = '<div id="pdfrender-custom-mapping-form"';
  $output .= drupal_render(drupal_get_form('pdfrender_custom_mapping_form', $file));
  $output .= '</div>';

  drupal_deliver_html_page($output);
}
