<?php

  $in = 'macs.txt';
  $out = 'out.txt';

  $nums = array();
  $row = 0;
  foreach (explode("\n", file_get_contents($in)) as $line) {
    if ($line != '') {
      $bytes = explode(":", $line);
      for ($byte = 0; $byte < 6; $byte++)
        $nums[$row][$byte] = hexdec($bytes[$byte]);
      $row++;
    }
  }
  array_multisort($nums);

  $out_text = "";
  foreach ($nums as $macnums) {
    $out_text .= sprintf("%02x:%02x:%02x:%02x:%02x:%02x\n", 
                   $macnums[0],
                   $macnums[1],
                   $macnums[2],
                   $macnums[3],
                   $macnums[4],
                   $macnums[5]);
  }

  file_put_contents($out, $out_text);

?>
