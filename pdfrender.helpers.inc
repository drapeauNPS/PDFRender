<?php
/**
 * @file
 * Collection of helper methods for storing different fields types such as
 * address fields or list_text.
 */


/*
 * Method that stores an address field to the passed in array.
 */

function _store_field_addressfield($key, $field, &$fields) {
  $address_fields = $field;
  if ((is_array($address_fields) && count($address_fields) > 0)) {
    foreach ($address_fields as $address_key => $address_val) {
      // Store all address fields that have a value set.
      if ($address_val != NULL) {
        $tmp = $key . '_' . $address_key;
        $fields[$tmp] = $address_val;
      }
    }
  }
}

/*
 * Method that stores a list of text field to the passed in array.
 */

function _store_field_list_text($key, $field, $is_metadata_wrapper, &$fields) {
  if ($is_metadata_wrapper) {
    $fields[$key] = $field;
  }
  else {
// Store all options selected in an array within $fields.
    $tmp = array();
    if (array_key_exists($key, $fields)) {
      $val = array_pop($fields[$key]);
      if (!is_array($val)) {
        $tmp[] = $val;
      }
    }
    array_push($tmp, isset($field['value']) ? $field['value'] : '');
    $fields[$key] = $tmp;
  }
}

/*
 * Method that stores a list of boolean field to the passed in array.
 */
function _store_field_list_boolean($key, $field, $is_metadata_wrapper, &$fields) {
  $val = $field;
  if ((!$is_metadata_wrapper) && isset($field['value'])) {
    $val = $field['value'];
  }
  $result = 'No';
  if ($val == 1) {
    $result = 'Yes';
  }
  $fields[$key] = $result;
}

/*
 * Method that stores a taxonomy term reference field to the passed in array.
 */

function _store_field_taxonomy_term($key, $field, $is_metadata_wrapper, &$fields) {
  if ($is_metadata_wrapper) {
    $fields[$key] = isset($field->name) ? $field->name : '';
  }
  else {
    if (isset($field['tid'])) {
      $term = taxonomy_term_load($field['tid']);
      if ((isset($term)) && isset($term->name)) {
        $fields[$key] = $term->name;
      }
    }
  }
}

/*
 * Method that stores a datetime field to the passed in array.
 */

function _store_field_datetime($key, $field, $is_metadata_wrapper, &$fields) {
  if ($is_metadata_wrapper) {
    $fields[$key] = isset($field) ? date('Y-m-d H:i:s', $field) : '';
  }
  else {
    $fields[$key] = isset($field['value']) ? $field['value'] : '';
  }
}

/*
 * Method that stores a standard field to the passed in array.
 */

function _store_field_field($key, $field, $is_metadata_wrapper, &$fields) {
  if ($is_metadata_wrapper) {
    $fields[$key] = isset($field) ? $field : '';
  }
  else {
    $fields[$key] = isset($field['value']) ? $field['value'] : '';
  }
}