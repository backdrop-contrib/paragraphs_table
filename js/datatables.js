(function ($) {
  Drupal.behaviors.datatables = {
    attach: function (context, settings) {
      $(document).ready( function () {
        $('.field-paragraphs-table-view').DataTable();
      } );
    }
  };
})(jQuery);
