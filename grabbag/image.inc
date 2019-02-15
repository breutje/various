<?php

  //
  // image processing functions
  //
  // Author: Bert Reuling
  //
  // LoadImage           - load image (uses extension to determine type)
  // AutoScale           - auto scale in-memory image
  // ApplyMask           - set alpha in image from alpha in mask (mask)
  // ApplyRoundedCorners - create rounded corners on image (pixel radius)
  // ImageJPEGHash       - save image as JPG, replace <hash> in filename with hash
  // ImagePNGHash        - save image as PNG, replace <hash> in filename with hash
  //

  //
  // resource LoadImage( string $filename )
  // --------------------------------------
  // Load image in memory from file or URL. This requires BMP.inc in order to read DIB images.
  //
  //
  // resource AutoScale( resource $image, integer $request_width, integer $request_height )
  // --------------------------------------------------------------------------------------
  // Auto scale in-memory image.
  //
  //
  // resource ApplyMask( resource $image, resouce $mask )
  // ----------------------------------------------------
  // Apply alpha of mask to image. The mask and image may have a different size, but same size
  // yields the best (accurate) results.
  //
  //
  // resource ApplyRoundedCorners ( resource $image, int $radius )
  // -------------------------------------------------------------
  // Apply rounded corners to image. The radius is in pixels. The color information of the corners is destroyed.
  //
  //
  // bool ImageCopyResampledAlpha ( resource $dst_image , resource $src_image , int $dst_x , int $dst_y , int $src_x , int $src_y , int $dst_w , int $dst_h , int $src_w , int $src_h , int $alpha )
  // -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Copy and resize part of an image with resampling, set alpha. If you can, avoid this function as it is very slow.
  //  The value of alpha is between 0 and 127. 0 indicates completely opaque while 127 indicates completely transparent.
  // 
  //
  // bool ImageJPEGHash ( resource $image [, string $filename [, int $quality ]] )
  // -----------------------------------------------------------------------------
  // ImageJPEG but replace <hash> in filename with (md5) hash of the file.
  //
  //
  // bool ImagePNGHash( resource $image [, string $filename [, int $quality ]] )
  // ---------------------------------------------------------------------------
  // ImagePNG but replace <hash> in filename with (md5) hash of the file.
  //
  //

  //
  // load image (in memory)
  //
  function LoadImage($location) {

    $extension = strtolower(substr($location, strrpos($location, ".")));
    if ($extension == ".jpg")
      $image = ImageCreateFromJPEG($location);
    else if ($extension == ".jpeg")
      $image = ImageCreateFromJPEG($location);
    else if ($extension == ".png")
      $image = ImageCreateFromPNG($location);
    else if ($extension == ".gif")
      $image = ImageCreateFromGIF($location);
    else if ($extension == ".bmp")
      $image = BMP::ImageCreateFromBMP($location);
    else
      $image = false;
    return $image;
  }

  //
  // auto scale image
  //
  function AutoScale($input_image, $request_width, $request_height) {

    //
    // create output image
    //
    $output_image = ImageCreateTrueColor($request_width, $request_height);
    ImageFill($output_image, 0, 0, ImageColorExactAlpha($output_image, 0, 0, 0, 127));
    ImageAlphaBlending($output_image, false);
    ImageSaveAlpha($output_image, true);

    $width = ImageSX($input_image);
    $height = ImageSY($input_image);
    $request_ratio = ($request_width / $request_height);
    $image_ratio = ($width / $height);
    if ($image_ratio > $request_ratio) {
      //
      // scale to height
      //
      $scale_width = round(($request_height * $width) / $height);
      $scale_height = $request_height;
    } else {
      //
      // scale to width
      //
      $scale_width = $request_width;
      $scale_height = round(($request_width * $height) / $width);
    }

    //
    // horizontal and vertical center
    //
    $center_x = round(($request_width  - $scale_width) / 2.0);
    $center_y = round(($request_height - $scale_height) / 2.0);

    ImageCopyResampled($output_image, $input_image, $center_x, $center_y, 0, 0, $scale_width, $scale_height, $width, $height);

    return $output_image;
  }

  //
  // Apply alpha of mask to image
  //
  function ApplyMask($image, $mask) {

    //
    // get the image sizes (may not be the same)
    //
    $image_width = ImageSX($image);
    $image_height = ImageSY($image);
    $mask_width = ImageSX($mask);
    $mask_height = ImageSY($mask);

    //
    // create blank, transparent output image
    //
    $output_image = ImageCreateTrueColor($image_width, $image_height);
    ImageFill($output_image, 0, 0, ImageColorExactAlpha($output_image, 0, 0, 0, 127));
    ImageAlphaBlending($output_image, true);
    ImageSaveAlpha($output_image, true);

    //
    // copy alpha from mask and color from image to out.
    //
    for ($x = 0; $x < $image_width; $x++) {
      for ($y = 0; $y < $image_height; $y++) {
        $mask_x = $x * $mask_width / $image_width;
        $mask_y = $y * $mask_height / $image_height;
        $RGB = ImageColorsForIndex($image, ImageColorAt($image, $x, $y));
        $ALPHA = ImageColorsForIndex($mask, ImageColorAt($mask, $mask_x, $mask_y));
        ImageSetPixel($output_image, $x, $y, ImageColorExactAlpha($output_image, $RGB['red'], $RGB['green'], $RGB['blue'], $ALPHA['alpha']));
      }
    }
    return $output_image;
  }

  //
  // create rounded corners by setting alpha to 127
  //
  function ApplyRoundedCorners($image, $radius) {

    $image_width = ImageSX($image);
    $image_height = ImageSY($image);
    $diameter = $radius * 2;

    //
    // make copy of image
    //
    $output_image = ImageCreateTrueColor($image_width, $image_height);
    ImageFill($output_image, 0, 0, ImageColorExactAlpha($output_image, 0, 0, 0, 127));
    ImageAlphaBlending($output_image, false);
    ImageSaveAlpha($output_image, true);
    ImageCopyResampled($output_image, $image, 0, 0, 0, 0, $image_width, $image_height, $image_width, $image_height);

    //
    // create arc (template)
    //
    $arc_image = ImageCreateTrueColor($radius, $radius);
    ImageFill($arc_image, 0, 0, ImageColorExactAlpha($arc_image, 0, 0, 0, 127));
    ImageAlphaBlending($arc_image, false);
    ImageSaveAlpha($arc_image, true);

    ImageFilledArc(
      $arc_image,
      $radius + 1,
      $radius + 1,
      $diameter,
      $diameter,
      180,
      270,
      ImageColorExactAlpha($arc_image, 0, 255, 0, 0),
      0
    );

    for ($y = 0; $y < $radius; $y++) {
      for ($x = 0; $x < ($radius - $y); $x++) {
        $ARC = ImageColorsForIndex($arc_image, ImageColorAt($arc_image, $x, $y));
        if ($ARC['green'] != 255) {
          ImageSetPixel($output_image, $x, $y, ImageColorExactAlpha($output_image, 255, 255, 255, 127));
          ImageSetPixel($output_image, $image_width - $x, $y, ImageColorExactAlpha($output_image, 255, 255, 255, 127));
          ImageSetPixel($output_image, $image_width - $x, $image_height - $y, ImageColorExactAlpha($output_image, 255, 255, 255, 127));
          ImageSetPixel($output_image, $x, $image_height - $y, ImageColorExactAlpha($output_image, 255, 255, 255, 127));
        }
      }
    }
    return $output_image;
  }

  //
  // copy resampled and apply alpha
  //
  function ImageCopyResampledAlpha($dst_image, $src_image, $dst_x, $dst_y, $src_x, $src_y, $dst_w, $dst_h, $src_w, $src_h, $alpha) {
    if ($alpha == 127)
      return false;
    else if ($alpha == 0)
      return ImageCopyResampled($dst_image, $src_image, $dst_x, $dst_y, $src_x, $src_y, $dst_w, $dst_h, $src_w, $src_h);
    else {
      $buffer_width = ImageSX($src_image);
      $buffer_height = ImageSY($src_image);
      $buffer_image = ImageCreateTrueColor($buffer_width, $buffer_height);
      ImageFill($buffer_image, 0, 0, ImageColorExactAlpha($buffer_image, 0, 0, 0, 127));
      ImageAlphaBlending($buffer_image, true);
      ImageSaveAlpha($buffer_image, true);
      $alpha_factor = (127 - $alpha) / 127;
      for ($x = $src_x; $x < $src_x + $src_w; $x++) {
        for ($y = $src_y; $y < $src_y + $src_h; $y++) {
          $RGB = ImageColorsForIndex($src_image, ImageColorAt($src_image, $x, $y));
          $MASK['alpha'] = (1.0 - (((127 - $RGB['alpha']) / 127) * $alpha_factor)) * 127;
          ImageSetPixel($buffer_image, $x, $y, ImageColorExactAlpha($buffer_image, $RGB['red'], $RGB['green'], $RGB['blue'], $MASK['alpha']));
        }
      }
      return ImageCopyResampled($dst_image, $buffer_image, $dst_x, $dst_y, $src_x, $src_y, $dst_w, $dst_h, $src_w, $src_h);
    }
  }


  //
  // save image as JPEG (replace <hash> in filename)
  //
  function ImageJPEGHash($image, $filename, $quality) {
    $charset = "0123456789abcdef";

    $hash = 'tmp.';
    for ($n = 0; $n < 32; $n++)
      $hash .= substr($charset, mt_rand(0, 0x0F), 1);
    $tmp_filename = str_replace('<hash>', $hash, $filename);
    $result = ImageJPEG($image, $tmp_filename, $quality);
    $hash = md5_file($tmp_filename);
    $hash_filename = str_replace('<hash>', $hash, $filename);
    rename($tmp_filename, $hash_filename);
    return $result;
  }

  //
  // save image as PNG (replace <hash> in filename)
  //
  function ImagePNGHash($image, $filename, $quality) {
    $charset = "0123456789abcdef";

    $hash = 'tmp.';
    for ($n = 0; $n < 32; $n++)
      $hash .= substr($charset, mt_rand(0, 0x0F), 1);
    $tmp_filename = str_replace('<hash>', $hash, $filename);
    $result = ImagePNG($image, $tmp_filename, $quality);
    $hash = md5_file($tmp_filename);
    $hash_filename = str_replace('<hash>', $hash, $filename);
    rename($tmp_filename, $hash_filename);
    return $result;
  }

?>
