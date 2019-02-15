<?php

   //
   // strength - assess password strength [0.0 (strong) - 1.0 (weak)]
   //
   // 0.0 - 0.1  very strong
   // 0.1 - 0.2  very strong
   // 0.2 - 0.3  strong
   // 0.3 - 0.4  strong
   // 0.4 - 0.5  medium
   // 0.5 - 0.6  medium
   // 0.6 - 0.7  weak
   // 0.7 - 0.8  weak
   // 0.8 - 0.9  very weak
   // 0.9 - 1.0  very weak
   //
   // $Id: strength.inc 73 2010-11-16 11:28:13Z bert $
   //
   function strength($password, $minimum = 8, $maximum = 20, $contributing = 50) {

      //
      // case sensitive - count upper and lowercase letters, numbers and symbols
      //
      $ingroup = false;
      $previous = '\b';
      $length = strlen($password);
      $uppers = $lowers = $numbers = $symbols = 0;
      $sequentual_groups = $sequential_max = $sequential = 0;

      for ($n = 0; $n < $length; $n++) {

         //
         // count
         //
         if ($password[$n] >= 'A' && $password[$n] <= 'Z')
            $uppers++;
         else if ($password[$n] >= 'a' && $password[$n] <= 'z')
            $lowers++;
         else if ($password[$n] >= '0' && $password[$n] <= '9')
            $numbers++;
         else
            $symbols++;

         //
         // detect single character groups
         //
         if (strtoupper($password[$n]) == $previous) {
            if ($ingroup == false)
               $sequentual_groups++;
            $ingroup = true;
            $sequential++;
            if ($sequential >= $sequential_max)
               $sequential_max = $sequential;
         } else {
            $sequential = 0;
            $ingroup = false;
         }
         $previous = strtoupper($password[$n]);
      }

      //
      // calculate contributing values
      //
      $d_uppers = abs(0.25 - ($uppers / $length));
      $d_lowers = abs(0.25 - ($lowers / $length));
      $d_numbers = abs(0.25 - ($numbers / $length));
      $d_symbols = abs(0.25 - ($symbols / $length));
      $d_sequence = $sequential_max / $length;
      $d_groups = ($sequentual_groups * 2.0) / $length;

      //
      // worse if password is shorter than minimum
      //
      if ($length < $minimum)
         $d_length = log($minimum / $length); // contribute to weakness
      else
         $d_length = 0.0; // neutral

      //
      // calculate 'weakness' value (square root from the sum of the squares)
      //
      $weakness = sqrt(pow($d_uppers, 2) + pow($d_lowers, 2) + pow($d_numbers, 2) + pow($d_symbols, 2) + pow($d_sequence, 2) + pow($d_groups, 2) + pow($d_length, 2));

      //
      // decrease weakness if passwords are longer (linear with cutoff at maximum) : not more than contributing factor (50%)
      //
      if ($length > $minimum)
         $weakness -= ((($length - $minimum) > $maximum ? $maximum : $length - $minimum) / $maximum) * $weakness * ($contributing / 100);

      //
      // don't return values higher than 1 (irrelevant)
      //
      return (($weakness > 1.0) ? 1.0 : $weakness);
   }

?>
