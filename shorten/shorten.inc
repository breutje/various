 <?php

  //
  // shorten - reduce length of forward reference free texts
  //
  function shorten($text, $maximum, $ellipsis = "\xE2\x80\xA6") {

    if (strlen($text) <=  $maximum)
      return $text;
    $found = preg_match_all('/[^A-Z][\.!?] /', substr($text, 0, $maximum), $matches, PREG_OFFSET_CAPTURE);
    if ($found == 0)
      return substr($text, 0, ($maximum - strlen($ellipsis))) . $ellipsis;
    $length = 0;
    for ($match = $found - 1; $match >= 0; $match--) {
      $length = $matches[0][$match][1] + 2;
      if ($length <= $maximum)
        break;
    }
    return substr($text, 0, $length);
  }
?>
