<?php
  //
  // compact php code by stripping comments and whitespace and using gzip and base64_encode
  //
  $infile = @$argv[1];
  $outfile = @$argv[2];
  if ($argc != 3 || $infile == $outfile) {
    echo "Usage: php {$argv[0]} <input file> <output file>\n";
    exit(0);
  }
  echo "Processing $infile to $outfile\n";
  $data = "ob_end_clean();?>";
  $data .= php_strip_whitespace($infile);

  //
  // compress data
  //
  $data = gzcompress($data,9);

  //
  // encode in base64
  //
  $data = base64_encode($data);

  //
  // generate output text
  //
  $out = '<?php ob_start();$a=\'' . $data . '\';eval(gzuncompress(base64_decode($a)));$v=ob_get_contents();@ob_end_clean();?>';

  //
  // write output text
  //
  file_put_contents($outfile, $out);

  echo "finished\n";
?>
