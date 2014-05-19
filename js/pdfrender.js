(function ($) {

    jQuery(document).ready(function () {

    });

    /* Hide the custom mapping form is the user cancels out. */
    jQuery(".cancelmapping").live('click', function () {
        jQuery("#pdfrender-custom-mapping-form").html('');
    });
})(jQuery);