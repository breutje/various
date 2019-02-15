<?php
  $url = @$_REQUEST['url'];
  $url = "http://nu.nl/rss";
  if ($url == '') {
    echo "<ul><li>No url configured</li></ul>\n";
    exit(0);
  }
  $doc = new DOMDocument();
  $doc->load($url);
  $xpath = new DOMXpath($doc);
  echo "<ul>\n";
  foreach($xpath->query("/rss/channel/item") as $node) {
    $title = $xpath->query("title", $node)->item(0)->nodeValue;
    echo "<li>{$title}</li>\n";
  }
  echo "</ul>\n";
?>
