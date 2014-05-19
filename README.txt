*******************************************************
    README.txt for pdfrender.module for Drupal
*******************************************************

This module was created by Dan Drapeau, software developer at NorthPoint Digital, a consulting
firm based in NYC with offices in Boston and Philadelphia.

PDFRender makes use of the FPDM library (https://github.com/lsolesen/fpdf/blob/master/examples/form_filling/fpdm.php):
a free FDF/PDF merge tool written in PHP.

This tool enables a site administrator to upload an editable PDF document (containing form fields), specify a content
type, and create a custom mapping to render fields associated with the content type to any form field on the PDF.

*******************************************************
    Use Case - Where this module can come in handy
*******************************************************

Say you have a set of users that are athletes and they need to download a PDF with the personal information
they provided rendered on the document so they can print it out and mail it to their physician.  The athletes
had previously entered information such as their date of birth, name, height, weight, grade, etc. which was stored
in a content type like 'Student Athlete Info'.

As an administrator, you can create an editable PDF and specify a mapping so that the fields associated with the
content type get rendered on the form fields of the PDF.  You take the provided 'Download PDF Forms' block and place
it in a structure like a panel that has a node id in the url.

An athlete logs in the system and sees an available PDF on the panel/block and clicks on it to download the
file that has their information (stored in the node) rendered onto it.

*******************************************************
    Prerequisite work for PDF creation
*******************************************************

In order for the module to properly render data onto an uploaded PDF, the document needs to be in the proper format.

1. Use a tool such as Adobe Acrobat Pro (free trial available) to create a new PDF or modify an existing one and add
   form fields to the document.  You can also use Microsoft Word to create the initial document and add form fields on
   then use a free service like http://www.freepdfconvert.com to transform it into a PDF.
2. The PDF needs to be compatible with Adobe Acrobat 5.0 (or greater).  In order to do this, I recommend using
   PDFTK - http://www.pdflabs.com/tools/pdftk-the-pdf-toolkit/

   After installing the free version, you can run the program on the command line using:
   'pdftk <filename> output <new filename>'
3. The document that is output is the one you'll want to upload.

*******************************************************
    Using the PDFRender module
*******************************************************

Upload a PDF and creating a custom mapping

1. After enabling the module, click on Configuration and then 'PDF Render' which found in Content Authoring.
2. Select a PDF document from your filesystem and a content type from the drop down list and click 'Upload Form'.
   You'll see a new entry populated in the table.
3. Click on 'Create new mapping' to generate which fields associated with your content you'd like to be rendered
   on what form fields.  If no mapping is created, then content fields will only be rendered on the PDF if the
   field label happens to be be the same as the form field name.
4. Upon selecting 'Create new mapping' a table will pop up with PDF form fields on the left column and a drop down
   list for each of the available content type fields on the right column.
   Perhaps you have a PDF where you need to have an individuals' phone number rendered on the document when they go
   to download it.  Using this tool, you can map the form field with a content type field (say 'Cell Phone') and
   the value associated with it (say '111-111-1111') will be written onto the PDF.

Configuring a block for user download of PDF

In order for users on the site to download PDFs with their content rendered, you'll need to configure the 'Download
PDF Forms' block found at /admin/structure/block.  A node id associated with that user (and content type) needs to
be included in the url that has the PDF name in it.

The required path to force a download is the following:  /ajax/process_pdf/<filename>/<nodeid> where filename
is the name of the uploaded PDF.  The links to the PDF can be displayed however you'd like to configure and
present them to your users.

My recommendation is to create a panel with your content and setup an argument in order to pass the node id along.
You can then add the custom block included with the PDFRender module by adding content, selecting Miscellaneous, and
then the block included with this module.

Using the use case above as an example, you might have a panel set up containing a message instructing the athlete
to print out their personal information form and send it off to their physician.  Using a panel, you can setup an
argument to pass the node id (of the node being viewed) along - i.e. /athlete/health-information/%healthid and add
the custom block included in the module to the panel and pass the node id along to it.
i.e. /ajax/process_pdf/<filename>/%healthid:nid

A sample form ('template.pdf') is included within the lib/fpdm directory for testing.