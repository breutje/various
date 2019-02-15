<?php
   //
   // validate.php - validate xml using XMLSchema
   //
   // Usage: php validate.php <schema> <xml-document>
   //
   $schema_name = $argv[1];
   $file_name = $argv[2];

   $doc = new DOMDocument();
   $doc->load($file_name);
   $valid = $doc->schemaValidate($schema_name);
   if ($valid)
       echo "\"$file_name\" conforms to \"$schema_name\"\n";
   else
       echo "\"$file_name\" does not conform to \"$schema_name\"\n";
?>
