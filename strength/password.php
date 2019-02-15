<?php
   include 'strength.inc';

   // 0.0 - 0.1 very strong
   // 0.1 - 0.2 very strong
   // 0.2 - 0.3 strong
   // 0.3 - 0.4 strong
   // 0.4 - 0.5 medium
   // 0.5 - 0.6 medium
   // 0.6 - 0.7 weak
   // 0.7 - 0.8 weak
   // 0.8 - 0.9 very weak
   // 0.9 - 1.0 very weak

   $password = $argv[1];
   $value = strength($password);

   if ($value >= 0.0 && $value < 0.2)
      $class = "very strong";
   else if ($value >= 0.2 && $value < 0.4)
      $class = "strong";
   else if ($value >= 0.4 && $value < 0.6)
      $class = "medium";
   else if ($value >= 0.6 && $value < 0.8)
      $class = "weak";
   else if ($value >= 0.8 && $value <= 1.0)
      $class = "very weak";

   echo "[$password] ($class) = $value\n";

?>
