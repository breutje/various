<?php

  //
  //
  // aaaa-bbbb&shy;cccc
  //
  // aaaa-
  // bbbb
  // <glue>
  // cccc
  //
  $text = "Een tekst kan afgebroken worden op een woord, een koppelteken of een zacht koppelteken. Maar wat doen we met Meervoudigepersoonlijkheidsstoornissen of zelfs kinder~carnavals~optocht~voorbereidings~werk~zaam~heden~comité~leden (lingo)";

  //$text = str_replace('&shy;', "\xad", $text);
  $text = str_replace('~', '&shy;', $text);
  $text = str_replace('&shy;', ' ', $text);
  $words = barrage($text);

  print_r($words);

  //
  // werk<glue>word<glue>ver<glue>voeging
  // mi<glue>nis<glue>ters<glue>por<glue>te<glue>feuil<glue>le
  //

  function barrage($text) {
    return explode(' ', $text);
  }
?>
