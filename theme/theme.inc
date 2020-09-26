<?php

/**
 * @file
 * Theme defined for Paragraph table module.
 */

/**
 * Replacement for theme_field_multiple_value_form().
 *
 * Each field is printed in a separate cell.
 */
function theme_paragraphs_table_multiple_value_fields($variables) {
  $element = $variables['element'];
  $output = '';

  // Ensure default widget settings turn dragging off for field formatter
  // without custom settings.
  if (!isset($element['#custom_settings'])) {
    $element['#custom_settings'] = array(
      'nodragging' => TRUE,
      'title_on_top' => TRUE,
    );
  }

  if (isset($element['#cardinality']) && ($element['#cardinality'] > 1 || $element['#cardinality'] == FIELD_CARDINALITY_UNLIMITED)) {
    $table_id = drupal_html_id($element['#field_name'] . '_values');
    $order_class = $element['#field_name'] . '-delta-order';
    $required = !empty($element['#required']) ? theme('form_required_marker', array('element' => $element)) : '';
    $header = array(
      array(
        'data' => ''
      ),
    );
    $rows = array();

    // Sort items according to '_weight'.
    // Needed when the form comes back after preview or failed validation.
    $items = array();
    foreach (element_children($element) as $key) {
      if (!isset($element[$key]['#entity_type'])) {
        if ($key === 'add_more') {
          $add_more_button = &$element[$key];
        }
      }
      else {
        $items[] = &$element[$key];
      }
    }

    // No need to sort if nodragging is selected.
    if (!$element['#custom_settings']['nodragging']) {
      usort($items, '_field_sort_items_value_helper');

      // Add header for table dragging.
      $header = array(
        array(
          'data' => '',
          'class' => 'tabledrag',
        ),
      );
    }

    // Add the items as table rows.
    foreach ($items as $key => $item) {
      uasort($item, 'element_sort');
      $item['_weight']['#attributes']['class'] = array($order_class);

      $cells = array();

      // Add classes for dragging if needed.
      if (!$element['#custom_settings']['nodragging']) {
        $cells = array(
          array('data' => '', 'class' => 'field-multiple-drag'),
        );
      }

      foreach (element_children($item) as $field_name) {
        // Don't add the _weight.
        if (!$element['#custom_settings']['nodragging'] || !in_array($field_name, ['_weight', 'paragraph_bundle_title'])) {
          if (!isset($item[$field_name]['#access']) || $item[$field_name]['#access']) {
            // Only add the header once.
            if ($key == 0) {
              $header[] = array(
                'data' => '<label>' . t('!title', array('!title' => _field_paragraphs_table_get_title($item[$field_name]))) . '</label>',
                'class' => array('field-label', drupal_html_class($field_name)),
              );
            }
            _field_paragraphs_table_remove_title($item[$field_name]);
            $cells[] = array(
              'data' => $item[$field_name],
              'class' => drupal_html_class($field_name),
            );
          }
        }
      }
      // Mark rows as draggable if needed.
      if (!$element['#custom_settings']['nodragging']) {
        $rows[] = array(
          'data' => $cells,
          'class' => array('draggable'),
        );
      }
      else {
        $rows[] = array(
          'data' => $cells,
        );
      }
    }

    $output = array(
      '#prefix' => '<div class="form-item">',
      '#suffix' => '</div>',
    );

    if ($element['#custom_settings']['title_on_top']) {
      $output['title'] = array(
        '#prefix' => "<label class='form-item-title'>",
        '#markup' => t('!title !required', array(
          '!title' => $element['#title'],
          '!required' => $required,
        )),
        '#suffix' => '</label>',
      );
    }

    $output['paragraphs_table'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#weight' => 20,
      '#attributes' => array(
        'id' => $table_id,
        'class' => array(
          'field-multiple-table',
        ),
      ),
    );
    if (!empty($element['#description'])) {
      $output[] = array(
        '#prefix' => '<div class="description">',
        '#suffix' => '</div>',
        '#markup' => $element['#description'],
        '#weight' => 30,
      );
    }
    if (isset($add_more_button)) {
      $add_more_button['#weight'] = 40;
      $output[]
        = $add_more_button + array(
          '#prefix' => '<div class="clearfix">',
          '#suffix' => '</div>',
        );
    }

    $output = drupal_render($output);

    // Add table drag.
    if (!$element['#custom_settings']['nodragging']) {
      drupal_add_tabledrag($table_id, 'order', 'sibling', $order_class);
    }
  }
  else {
    foreach (element_children($element) as $key) {
      $output .= drupal_render($element[$key]);
    }
  }
  return $output;
}

/**
 * Helps find the title of the field, as it could be in several places.
 */
function _field_paragraphs_table_get_title($field) {
  $title = '';
  $required = FALSE;

  if (isset($field['#language']) && isset($field[$field['#language']])) {
    $language = $field['#language'];
    if (isset($field[$language]['#title'])) {
      $title = $field[$language]['#title'];
      $required = !empty($field[$language]['#required']);

    }
    elseif (isset($field[$language][0]['#title'])) {
      $title = $field[$language][0]['#title'];
      $required = !empty($field[$language][0]['#required']);
    }
    if (empty($title) && !empty($field[$language]['select']['#title'])) {
      $title = $field[$language]['select']['#title'];
    }
  }
  elseif (isset($field['#title'])) {
    $title = empty($field['#is_weight']) ? $field['#title'] : t('Order');
    $required = !empty($field['#required']);
  }
  elseif (isset($field['#value'])) {
    $title = $field['#value'];
    $required = !empty($field['#required']);
  }
  $required = $required ? theme('form_required_marker', array('element' => $field)) : '';
  return t('!title !required', array(
    '!title' => $title,
    '!required' => $required,
  ));
}

/**
 * Custom function _field_paragraphs_table_remove_title().
 */
function _field_paragraphs_table_remove_title(&$field) {
  if (isset($field['#language']) && isset($field[$field['#language']])) {
    $language = $field['#language'];
    if (isset($field[$language]['#title'])) {
      $field[$language]['#title_display'] = 'none';
    }
    elseif (isset($field[$language][0]['#title'])) {
      $field[$language][0]['#title_display'] = 'none';
    }
    if (!empty($field[$language]['select']['#title'])) {
      $field[$language]['select']['#title_display'] = 'none';
    }
    if (!empty($field[$language][0]["value"]["date"])) {
      $field[$language][0]["value"]["date"]['#title_display'] = 'none';
      $field[$language][0]['#title_display'] = 'none';
    }
  }
  elseif (isset($field['#title'])) {
    $field['#title_display'] = 'none';
  }
  elseif (isset($field['#value'])) {
    $field['#value'] = '';
  }
}
