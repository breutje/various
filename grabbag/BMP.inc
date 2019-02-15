<?php

  //
  // Read DIB or Device Independent Bitmaps
  //
  // Read 1, 4, 8, 24 or 32 bit BMP files
  // Author: de77
  //
  class BMP {

    function __construct() {
      $this->error = '';
    }

    public static function ImageCreateFromBMP($filename) {

      //
      // read header    
      //
      $f = fopen($filename, "rb");
      $header = fread($f, 54);
      $header = unpack('c2identifier/Vfile_size/Vreserved/Vbitmap_data/Vheader_size/' .
                       'Vwidth/Vheight/vplanes/vbits_per_pixel/Vcompression/Vdata_size/'.
                       'Vh_resolution/Vv_resolution/Vcolors/Vimportant_colors', $header);
   
      if ($header['identifier1'] != 66 || $header['identifier2'] != 77) {
        $this->error = 'Not a valid bmp file';
        return false;
      }
       
      if (!in_array($header['bits_per_pixel'], array(24, 32, 8, 4, 1))) {
        $this->error = 'Only 1, 4, 8, 24 and 32 bit BMP images are supported';
        return false;
      }
       
      $bps = $header['bits_per_pixel'];
      $wid2 = ceil(($bps / 8 * $header['width']) / 4) * 4;
      $colors = pow(2, $bps);
   
      $wid = $header['width'];
      $hei = $header['height'];
   
      $img = ImageCreateTrueColor($header['width'], $header['height']);
   
      //
      // read palette
      //
      if ($bps < 9) {
        for ($i=0; $i<$colors; $i++) {
          $palette[] = self::undword(fread($f, 4));
        }
      } else {
        if ($bps == 32) {
          ImageAlphaBlending($img, false);
          ImageSaveAlpha($img, true);         
        }
        $palette = array();
      }   
      
      //
      // read pixels    
      //
      for ($y = $hei - 1; $y >= 0; $y--) {
        $row = fread($f, $wid2);      
        $pixels = self::str_split2($row, $bps, $palette);
        for ($x = 0; $x < $wid; $x++) {
          self::MakePixel($img, $x, $y, $pixels[$x], $bps);
        }
      }
      fclose($f);           
      
      return $img;
    }
   
    private static function str_split2($row, $bps, $palette) {
      switch ($bps) {
        case 32:
        case 24: return str_split($row, $bps / 8);
        case  8: $out = array();
                 $count = strlen($row);            
                 for ($i = 0; $i < $count; $i++) {               
                   $out[] = $palette[ord($row[$i])];
                 }            
                 return $out;      
        case  4: $out = array();
                 $count = strlen($row);            
                 for ($i = 0; $i < $count; $i++) {
                   $roww = ord($row[$i]);                  
                   $out[] = $palette[($roww & 240) >> 4   ];
                   $out[] = $palette[($roww & 15)       ];
                 }            
                 return $out;
        case  1: $out = array();
                 $count = strlen($row);            
                 for ($i = 0; $i < $count; $i++) {
                   $roww = ord($row[$i]);                  
                   $out[] = $palette[($roww & 128) >> 7];
                   $out[] = $palette[($roww & 64)  >> 6];
                   $out[] = $palette[($roww & 32)  >> 5];
                   $out[] = $palette[($roww & 16)  >> 4];
                   $out[] = $palette[($roww & 8)   >> 3];
                   $out[] = $palette[($roww & 4)   >> 2];
                   $out[] = $palette[($roww & 2)   >> 1];
                   $out[] = $palette[($roww & 1)       ];
                 }            
                 return $out;               
      }
    }
   
    private static function MakePixel($img, $x, $y, $str, $bps) {
      switch ($bps) {
        case 32: $a = ord($str[0]);
                 $b = ord($str[1]);
                 $c = ord($str[2]);
                 $d = 256 - ord($str[3]); //TODO: gives imperfect results
                 $pixel = $d * 256 * 256 * 256 + $c * 256 * 256 + $b * 256 + $a;
                 ImageSetPixel($img, $x, $y, $pixel);
                 break;
        case 24: $a = ord($str[0]);
                 $b = ord($str[1]);
                 $c = ord($str[2]);
                 $pixel = $c*256*256 + $b*256 + $a;
                 ImageSetPixel($img, $x, $y, $pixel);
                 break;               
        case  8:
        case  4:
        case  1: ImageSetPixel($img, $x, $y, $str);
                 break;
      }
    }
   
    private static function undword($n) {
      $r = unpack("V", $n);
      return $r[1];
    }
   
  }

?>
