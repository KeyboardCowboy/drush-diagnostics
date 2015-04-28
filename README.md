# Drush Diagnostics
Drush plugins to generate diagnostic reports of your Drupal site.

## Field Reports
### `drush diag-field-use-report`  

Generate a report listing all fields and number of records per field.
  
#### Options
`--instance` : Count records by field instance instead of grouping by field.

`--cols` : Add columns from the field_config table.  (I.e. `--cols=type` to get the field type)

*Supports output formats*

#### Examples
      'drush dfur' => 'List all fields and number of records for each.',
      'drush dfur --instance' => 'List all fields by entity type and bundle and number of records per instance.'
      'drush dfur --cols=type' => 'Add the "type" col from the field_config table to get the field type.'
