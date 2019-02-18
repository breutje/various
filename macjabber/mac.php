<?php

  //
  // macjabber - try to use mac address as easy to use unique identifier that may be pronounced
  //

  $macs = array(
    'D0:50:99:83:DE:81',
    'D0:50:99:83:DD:FB',
    'D0:50:99:83:DE:7B',
    '00:50:56:8b:09:54',
    'D8:CB:8A:38:8E:4B',
    '40:8D:5C:1A:DC:DF',
    'AC:22:0B:52:C9:C4'
  );

  $manufacturer = array(
    'D0:50:99' => 'A',
    '00:50:56' => 'B',
    '40:8D:5C' => 'C',
    'AC:22:0B' => 'D'
  );

  $jabber[] = array(
    "ka", "hi", "tu", "ne",
    "ho", "ya", "ma", "te",
    "fu", "ko", "ke", "re",
    "ru", "ro", "ni", "wo"
  );

  // 46 > 2^5 = 32
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

  // 46 > 2^5 = 32
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

  // 105 > 2^6 = 64
  $jabberwock = array(
    'ba', 'be', 'bi', 'bo', 'bu',
    'ca', 'ce', 'ci', 'co', 'cu',
    'da', 'de', 'di', 'do', 'du',
    'fa', 'fe', 'fi', 'fo', 'fu',
    'ga', 'ge', 'gi', 'go', 'gu',
    'ha', 'he', 'hi', 'ho', 'hu',
    'ja', 'je', 'ji', 'jo', 'ju',
    'ka', 'ke', 'ki', 'ko', 'ku',
    'la', 'le', 'li', 'lo', 'lu',
    'ma', 'me', 'mi', 'mo', 'mu',
    'na', 'ne', 'ni', 'no', 'nu',
    'pa', 'pe', 'pi', 'po', 'pu',
    'qa', 'qe', 'qi', 'qo', 'qu',
    'ra', 're', 'ri', 'ro', 'ru',
    'sa', 'se', 'si', 'so', 'su',
    'ta', 'te', 'ti', 'to', 'tu',
    'va', 've', 'vi', 'vo', 'vu',
    'wa', 'we', 'wi', 'wo', 'wu',
    'xa', 'xe', 'xi', 'xo', 'xu',
    'ya', 'ye', 'yi', 'yo', 'yu',
    'za', 'ze', 'zi', 'zo', 'zu',
  );

  //
  // 64 pairs for 6 bits per pair (2^6=64)
  //
  $jabberwocky = array(
    'BA', 'BE', 'BI', 'BO', 'BU',
    'CA', 'CE', 'CI', 'CO', 'CU',
    'DA', 'DE', 'DI', 'DO', 'DU',
    'FA', 'FE', 'FI', 'FO', 'FU',
    'GA', 'GE', 'GI', 'GO', 'GU',
    'HA', 'HE', 'HI', 'HO', 'HU',
    'JA', 'JE', 'JI', 'JO', 'JU',
    'KA', 'KE', 'KI', 'KO', 'KU',
    'LA', 'LE', 'LI', 'LO', 'LU',
    'MA', 'ME', 'MI', 'MO', 'MU',
    'NA', 'NE', 'NI', 'NO', 'NU',
    'PA', 'PE', 'PI', 'PO', 'PU',
    'RA', 'RE', 'RI', 'RO'
  );

  echo "jabberwock contains " . count($jabberwock) . " pairs\n";

  if (1) {
    $tests = array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 255, 254, 126, 127, 128);
    foreach ($tests as $test) {
      printf("%3d %02X %s [%s][%s]\n", $test, $test, binbyte($test), binbyte2($test), binbyte3($test));
    }
  }

  //
  // mac is 48 bits code is 6 bytes. n * 6 = m * 8
  //
  foreach ($macs as $mac) {
    $word = jabberify2($mac);
    echo "{$mac} = [$word]\n";
  }

  $code = 6;
  $lcm = ($code * 8) / gcd($code, 8); // Least common multiple
  $bytes = $lcm / 8;
  $codes = $lcm / $code;
  echo "lcm = $lcm\n";
  echo "$bytes bytes can contain $codes codes of $code bits each\n";

  echo "jabberwocky contains " . count($jabberwocky) . " pairs\n";

  //
  // jabberify - make a mac address pronounceable
  //
  // 48 = 16 x 3
  //
  function jabberify($mac) {
    global $jabberwock;

    $wock = '';
    $bytes = explode(":", $mac);
    for ($group = 0; $group < 6; $group += 3) {
      $c1 = hexdec($bytes[$group + 0]) >> 2;
      $c2 = ((hexdec($bytes[$group + 0]) & 0x03) << 4) | ((hexdec($bytes[$group + 1]) & 0xf0) >> 4);
      $c3 = ((hexdec($bytes[$group + 1]) & 0x0f) << 2) | ((hexdec($bytes[$group + 2]) & 0xc0) >> 4);
      $c4 = (hexdec($bytes[$group + 3]) & 0x3f);
      $w1 = $jabberwock[$c1];
      $w2 = $jabberwock[$c2];
      $w3 = $jabberwock[$c3];
      $w4 = $jabberwock[$c4];
      $wock .= "{$w1}{$w2}{$w3}{$w4}";
    }
    return $wock;
  }

  //
  // 24 = 4 x 6 = 3 x 8
  //
  function jabberify2($mac) {
    global $jabberwocky;

    unset($wock);
    $wock = '';
    unset($bytes);
    //echo "******** mac=[$mac]\n";
    
    foreach (explode(":", $mac) as $byte)
      $bytes[] = hexdec($byte);
    //print_r($bytes);
    

    //
    // get manufacturer
    //
    $manufacturer = 'A';

    //         |         |
    // 33333333|4444 4444|55 555555 bytes
    // 11111111|1111 1111|11 111111 bits
    // 00000011|1111 2222|22 333333 sextets
    //         |         |
    

    //         |         |
    // 33333333|4444 4444|55 555555 bytes
    //
    // 00000011 1111 2222 22 333333
    //
    $sextet[0] = ($bytes[3] >> 2);
    $sextet[1] = ($bytes[3] & 0x03) << 4 | ($bytes[4] & 0xF0) >> 4;
    $sextet[2] = ($bytes[4] & 0x0F) << 2 | ($bytes[5] & 0xC0) >> 6;
    $sextet[3] = ($bytes[5] & 0x3F);
    
if (false) {
    printf("|.......|.......|.......\n");
    for ($n = 0; $n < 3; $n++)
      printf("|%02X     ", $bytes[3 + $n]);
    printf("\n");

    for ($n = 0; $n < 3; $n++)
      printf("%08b", $bytes[3 + $n]);
    printf("\n");

    for ($n = 0; $n < 4; $n++)
      printf("%06b", $sextet[$n]);
    printf("\n");

    printf("|.....|.....|.....|.....\n");
}

    for ($n = 0; $n < 4; $n++)
      $wock .= $jabberwocky[$sextet[$n]];

    return "{$manufacturer}-{$wock}";

  }

  //
  // gcd - greatest common divisor
  //
  function gcd($a, $b) {
    if ($a == 0 || $b == 0)
      return abs( max(abs($a), abs($b)) );
        
    $r = $a % $b;
    return ($r != 0) ?  gcd($b, $r) : abs($b);
  }

  //
  // binbyte - return byte as binary
  //
  function binbyte($n) {
    $nibble = array(
      '0000', '0001', '0010', '0011',
      '0100', '0101', '0110', '0111',
      '1000', '1001', '1010', '1011',
      '1100', '1101', '1110', '1111'
    );
    
    if ($n < 0 || $n > 0xff)
       return '????????';
    else
      return $nibble[(($n & 0xf0) >> 4)] . $nibble[$n & 0x0f];
  }

  //
  // test each bit
  //
  function binbyte2($n) {
    $result = '';
    $mask = 0x80;
    for ($bit = 8; $bit > 0; $bit--) {
      $result .= (($n & $mask) == 0) ? '0' : '1';
      $mask >>= 1;
    }
    return $result;

  }

  //
  // test each bit (clever, but less readable)
  //
  function binbyte3($n) {
    $result = '';
    for ($mask = 0x80; $mask != 0; $mask >>= 1)
      $result .= (($n & $mask) == 0) ? '0' : '1';
    return $result;
  }

  //
  // nibbles
  //
  function macjabber($n) {
  }
  // for (int i = 0; i < 6; i++) {
  //   int hi = (mac_address[i] >> 4) & 0x0F;
  //   int lo = mac_address[i] & 0x0F;
  //   printf("%s%s", trans[hi], trans[lo]);
  //   if (i < 5)
  //     printf(" ");
  // }
  // printf("\n");


?>
