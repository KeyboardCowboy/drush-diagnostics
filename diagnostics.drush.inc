<?php
/**
 * @file
 * Generate diagnostic reports about your site.
 */
/**
 * Implements hook_drush_command().
 */
function diagnostics_drush_command() {
  $items['diag-field-use-report'] = array(
    'description' => 'Generate a report listing all fields, the module implementing them, their instances and number of DB rows per instance.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_DATABASE,
    'aliases' => array('dfur'),
    'options' => array(
      'cols' => 'Add columns from the field_config table.',
      'instance' => 'List usage by field instance.',
      'format' => 'Set the output format for the report. (Defaults to table.)',
    ),
    'examples' => array(
      'drush dfur' => 'List all fields with type and number of records.',
      'drush dfur --instance' => 'List all fields by entity type and bundle with field type and number of records.',
    ),
  );

  return $items;
}

/**
 * Command callback.
 * drush diag-field-use-report
 */
function drush_diagnostics_diag_field_use_report() {
  $data = array();

  // Create a list of fields to fetch from field_config
  $fields = array('field_name');
  if ($cols = drush_get_option('cols', '')) {
    $fields = array_merge($fields, explode(',', $cols));
  }

  // Get data from field_config
  $res = db_select('field_config', 'fc')
    ->fields('fc', $fields)
    ->condition('deleted', 0)
    ->orderBy('field_name')
    ->execute();

  while ($row = drush_db_fetch_object($res)) {
    $data[$row->field_name] = (array) $row;

    // Add data by field instance
    if (drush_get_option('instance')) {
      $query = db_select("field_data_{$row->field_name}", $row->field_name)
        ->fields($row->field_name, array('entity_type', 'bundle'))
        ->groupBy('entity_type')
        ->groupBy('bundle');

      $query->addExpression('COUNT(*)', 'count');
      $count_res = $query->execute();

      while ($count_row = $count_res->fetchObject()) {
        $name = "{$row->field_name}:{$count_row->entity_type}:{$count_row->bundle}";
        $data[$name] = $data[$row->field_name];
        $data[$name]['instance'] = "{$count_row->entity_type}:{$count_row->bundle}";
        $data[$name]['records'] = $count_row->count;
      }
      unset($data[$row->field_name]);
    }
    // Add data by field base
    else {
      $rows = db_select("field_data_{$row->field_name}", $row->field_name)
        ->fields($row->field_name)
        ->countQuery()
        ->execute()
        ->fetchField();

      $data[$row->field_name]['records'] = $rows;
    }
  }

  // Set the metadata for the output formatter
  $meta = array(
    'include-field-labels' => TRUE,
  );

  // Capitalize column headers
  $item = reset($data);
  foreach (array_keys($item) as $col) {
    $meta['field-labels'][$col] = strtoupper($col);
  }

  drush_print_format($data, 'table', $meta);
}
