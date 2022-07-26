/**
 * @file
 * Javascript file for Paragraph table module.
 */

(function ($) {
  Backdrop.behaviors.datatables = {
    attach: function (context, settings) {
      $(document).ready( function () {
        $('.field-paragraphs-table-view').DataTable();
      });
    }
  };
})(jQuery);
