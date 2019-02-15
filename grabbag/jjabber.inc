<?php

  //
  // produce a (random) nonsense word
  //
  function jjabber($syllables = 4) {
    $hiragana = array(
      'あ', 'い', 'う', 'え', 'お',
      'か', 'き', 'く', 'け', 'こ',
      'さ', 'し', 'す', 'せ', 'そ',
      'た', 'ち', 'つ', 'て', 'と',
      'な', 'に', 'ぬ', 'ね', 'の',
      'は', 'ひ', 'ふ', 'へ', 'ほ',
      'ま', 'み', 'む', 'め', 'も',
      'や', 'わ', 'ゆ', 'を', 'よ',     // わ and を should be on a separate line
      'ら', 'り', 'る', 'れ', 'ろ',
      'ん'
    );

    $translit = array(
       'a',   'i',   'u',   'e',   'o',
      'ka',  'ki',  'ku',  'ke',  'ko',
      'sa', 'shi',  'su',  'se',  'so',
      'ta', 'chi', 'tsu',  'te',  'to',
      'na',  'ni',  'nu',  'ne',  'no',
      'ha',  'hi',  'fu',  'he',  'ho',
      'ma',  'mu',  'mu',  'me',  'mo',
      'ya',  'wa',  'yu',  'wo',  'yo', // wa and wo should be on a separate line
      'ra',  'ri',  'ru',  're',  'ro',
      'n'
    );

    $jsyl = $translit;
    //$jsyl = $hiragana;
    $word = '';
    for ($s = 0; $s < $syllables; $s++) {
      $row = mt_rand(0, 8);
      $column = mt_rand(0, 4);
      $word .= $jsyl[$row * 5 + $column];
    }
    if (mt_rand(0, 9) == 7)
      $word .= $jsyl[count($jsyl) - 1];
    return $word;
  }

?>
