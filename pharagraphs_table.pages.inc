<?php
/*
 * @TODO ALL
 */
/**
 * paragraphs item view page.
 */
function paragraphs_items_page_view($paragraphs_items) {

  $field_paragraphs = paragraphs_field_get_entity($paragraphs_items);
  if (empty($field_paragraphs)) {
    return;
  }
  //@todo view in mode table vertical or horizontal
  return $field_paragraphs->view('full');
}

/**
 * Form for editing a paragraphs item.
 * @todo implement hook_forms().
 */
function paragraphs_items_form($form, &$form_state, $paragraphs_items) {


  $form['actions'] = array('#type' => 'actions', '#weight' => 50);
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
  );
  return $form;
}

/**
 * Validation callback.
 */
function paragraphs_items_form_validate($form, &$form_state) { }

/**
 * Submit builder. Extracts the form values and updates the entity.
 */
function paragraphs_items_form_submit_build_field_collection($form, $form_state) {
  return $form_state;
}

/**
 * Submit callback that permanently saves the changes to the entity.
 */
function paragraphs_items_form_submit($form, &$form_state) {

}

/**
 * Form for deleting a field collection item.
 */
function paragraphs_items_delete_confirm($form, &$form_state, $paragraphs_items) {
  $form_state += array('paragraphs_items' => $paragraphs_items);
  return confirm_form($form,
    t('Are you sure you want to delete %label?', array('%label' => $paragraphs_items->label())),
    $paragraphs_items->path(),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit callback for deleting a field collection item.
 */
function paragraphs_items_delete_confirm_submit($form, &$form_state) {
  $paragraphs_items = $form_state['paragraphs_items'];
  entity_delete('paragraphs_items', $paragraphs_items->item_id);
  backdrop_set_message(t('%label has been deleted.', array('%label' => backdrop_ucfirst($paragraphs_items->label()))));
  $form_state['redirect'] = '<front>';
}

/**
 * Add a new paragraphs item.
 *
 */
function paragraphs_items_add($field_name, $entity_type, $entity_id, $revision_id = NULL, $langcode = NULL) {
  $info = entity_get_info();
  if (!isset($info[$entity_type])) {
    return MENU_NOT_FOUND;
  }
  $result = entity_load($entity_type, array($entity_id));
  $entity = reset($result);
  if (!$entity) {
    return MENU_NOT_FOUND;
  }
  // Ensure the given entity is of a bundle that has an instance of the field.
  list($id, $rev_id, $bundle) = entity_extract_ids($entity_type, $entity);
  $instance = field_info_instance($entity_type, $field_name, $bundle);
  if (!$instance) {
    return MENU_NOT_FOUND;
  }

  // Check field cardinality.
  $field = field_info_field($field_name);
  $langcode = !empty($field['translatable'])?entity_language($entity_type, $entity) : LANGUAGE_NONE;

  if (!($field['cardinality'] == FIELD_CARDINALITY_UNLIMITED || !isset($entity->{$field_name}[$langcode]) || count($entity->{$field_name}[$langcode]) < $field['cardinality'])) {
    backdrop_set_message(t('Too many items.'), 'error');
    return '';
  }

  $paragraphs_items = entity_create('paragraphs_items', array('field_name' => $field_name));
  // Do not link the field collection item with the host entity at this point,
  // as during the form-workflow we have multiple field collection item entity
  // instances, which we don't want link all with the host.
  // That way the link is going to be created when the item is saved.
  $paragraphs_items->setHostEntity($entity_type, $entity, $langcode, FALSE);

  $label = $paragraphs_items->translatedInstanceLabel();
  $title = ($field['cardinality'] == 1) ? $label : t('Add new !instance_label', array('!instance_label' => $label));
  backdrop_set_title($title);

  // Make sure the current user has access to create a field collection item.
  global $user;
  $account = user_load($user->uid);
  if (!entity_access('create', 'paragraphs_items', $paragraphs_items, $account)) {
    return MENU_ACCESS_DENIED;
  }
  return backdrop_get_form('paragraphs_items_form', $paragraphs_items);
}
