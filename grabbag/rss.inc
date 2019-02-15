<?php

  //
  // RSS2.0
  //
  class rss {

    function __construct() {
      $this->doc = new DOMDocument();
      $this->doc->formatOutput = true;
      $this->doc->preserveWhiteSpace = false ;
      $this->doc->encoding = "UTF-8";
      $this->topinsert = true;
      $this->create_guid = true;
      $this->source = '';
    }

    function __destruct() {
    }

    //
    // set flag to 'false' to insert new items (and duplicate) at bottom. 'true' (default) inserts at top.
    //
    public function settop($flag) {
      $this->topinsert = $flag;
    }

    //
    // set flag to 'false' for feed without automatic generated guid's. 'true' (default) generates new guid for each new item.
    //
    function setguid($flag) {
      $this->create_guid = $flag;
    }

    //
    // load RSS from file or URL
    //
    public function load($filename) {
      $this->source = $filename;
      $result = $this->doc->load($this->source);
      $this->xpath = new DOMXpath($this->doc);
      $channel = $this->channel = $this->xpath->query("/rss/channel")->item(0);
      if ($channel === false)
        $result = false;
      return $result;
    }

    //
    // save RSS to file
    //
    public function save($filename) {
      return $this->doc->save($filename);
    }

    //
    // dump internal XML tree back into a string
    //
    public function saveXML() {
      return $this->doc->saveXML();
    }

    //
    // get source filename or url
    //
    public function source() {
      return $this->source;
    }

    //
    // get array of all titles (strings)
    //
    public function titles() {
      $results = array();
      $itemnodes = $this->xpath->query("/rss/channel/item");
      foreach ($itemnodes as $node)
        $results[] = $this->xpath->query("title", $node)->item(0)->nodeValue;
      return $results;
    }

    //
    // get array of all guids (strings)
    //
    public function guids() {
      $results = array();
      $itemnodes = $this->xpath->query("/rss/channel/item");
      foreach ($itemnodes as $node)
        $results[] = $this->xpath->query("guid", $node)->item(0)->nodeValue;
      return $results;
    }

    //
    // get array of all item (nodes)
    //
    public function items() {
      return $this->xpath->query("/rss/channel/item");
    }

    //
    // get or set title of item (node)
    //
    public function title($node, $data = NULL) {
      $result = $this->value($node, "title", true, $data);
      return $result;
    }

    //
    // get or set description of item (node)
    //
    public function description($node, $data = NULL) {
      $result = $this->value($node, "description", true, $data);
      return $result;
    }
      
    //
    // get or set (new) element of item (node)
    //
    public function value($node, $element, $cdata = true, $data = NULL) {
      if ($data == NULL)
        $result = @$this->xpath->query($element, $node)->item(0)->nodeValue;
      else {
        $data = preg_replace('/[\x00-\x1F\x7F]/', '', $data);
        $current_node = $this->xpath->query($element, $node)->item(0);
        $element_node = $this->doc->createElement($element);
        $cdata_node = $this->doc->createCDATASection($data);
        if ($cdata == true)
          $element_node->appendChild($cdata_node);
        else
          $element_node->nodeValue = $data;
        if ($current_node == NULL)
          $node->appendChild($element_node);
        else
          $node->replaceChild($element_node, $current_node);
        $result = $data;
      }
      return $result;
    }

    //
    // create (rather than load)
    //
    public function create($title, $description, $namespace = NULL, $uri = NULL, $cdata = false) {
      $this->xpath = new DOMXpath($this->doc);
      $this->rss = $this->doc->createElement("rss");
      if ($namespace != NULL && $uri != NULL)
        $this->rss->setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:${namespace}", $uri);
      $this->rss->setAttribute("version", "2.0");
      $this->doc->appendChild($this->rss);
      $this->channel = $this->doc->createElement("channel");
      $this->rss->appendChild($this->channel);

      if ($cdata == true) {
        $cdata_node = $this->doc->createCDATASection($title);
        $node = $this->doc->createElement("title");
        $node->appendChild($cdata_node);
      } else
        $node = $this->doc->createElement("title", $title);
      $this->channel->appendChild($node);
      if ($cdata == true) {
        $cdata_node = $this->doc->createCDATASection($description);
        $node = $this->doc->createElement("description");
        $node->appendChild($cdata_node);
      } else
        $node = $this->doc->createElement("description", $description);
      $this->channel->appendChild($node);
    }

    //
    // image (add to channel) NOTE: url, link and title are mandatory. image by itself is not
    //
    public function image($url = "", $link = "", $title = "", $desciption = null, $width = null, $height = null) {
      $image_node = $this->doc->createElement("image");
      $node = $this->doc->createElement("url", $url);
      $image_node->appendChild($node);
      $node = $this->doc->createElement("link", $link);
      $image_node->appendChild($node);
      $node = $this->doc->createElement("title", $title);
      $image_node->appendChild($node);
      if ($desciption != null) {
        $node = $this->doc->createElement("description", $desciption);
        $image_node->appendChild($node);
      }
      if ($width != null) {
        $node = $this->doc->createElement("width", $width);
        $image_node->appendChild($node);
      }
      if ($height != null) {
        $node = $this->doc->createElement("height", $height);
        $image_node->appendChild($node);
      }
      $this->channel->appendChild($image_node);
    }

    //
    // create new item
    //
    public function item($title, $description = NULL) {
      $node = $this->doc->createElement("item");
      $this->value($node, "title", true, $title);
      if ($description != NULL)
        $this->value($node, "description", true, $description);
      if ($this->create_guid == true) {
        $guid = $this->doc->createElement("guid", $this->create_uuid());
        $guid->setAttribute("isPermaLink", "false");
        $node->appendChild($guid);
      }
      if ($this->topinsert) {
        $first = $this->xpath->query("/rss/channel/item")->item(0);
        if ($first == NULL)
          $this->channel->appendChild($node);
        else
          $first->parentNode->insertBefore($node, $first);
      } else
        $this->channel->appendChild($node);
      return $node;
    }

    //
    // add enclosure to item
    //
    public function enclosure($item_node, $url, $length, $type) {
      $node = $this->doc->createElement("enclosure");
      $node->setAttribute("url", $url);
      $node->setAttribute("length", $length);
      $node->setAttribute("type", $type);
      $item_node->appendChild($node);
      return $item_node;
    }

    //
    // add manual guid to item
    //
    public function guid($item_node, $guid) {
      $node = $this->doc->createElement("guid", $guid);
      $node->setAttribute("isPermaLink", "false");
      $item_node->appendChild($node);
      return $item_node;
    }

    //
    // duplicate item with new guid
    //
    public function duplicate($guid) {
      $node = $this->xpath->query("/rss/channel/item/guid[text()='$guid']")->item(0);
      if ($node == NULL)
        return false;
      $new_guid = $this->create_uuid();
      $copy = $node->parentNode->cloneNode(true);
      $this->xpath->query("guid", $copy)->item(0)->nodeValue = $new_guid;

      if ($this->topinsert) {
        $first = $this->xpath->query("/rss/channel/item")->item(0);
        $first->parentNode->insertBefore($copy, $first);
      } else
        $this->channel->appendChild($copy);

      return $new_guid;
    }

    //
    // find item by guid
    //
    public function find($guid) {
      return $this->xpath->query("/rss/channel/item/guid[text()='$guid']")->item(0)->parentNode;
    }

    //
    // move item 'guid' one place up
    //
    public function up($guid) {
      $previous = null;
      foreach ($this->items() as $node) {
        if (($this->xpath->query("guid", $node)->item(0)->nodeValue == $guid) && ($previous != null)) {
          $node->parentNode->insertBefore($node, $previous);
          break;
        } else
        $previous = $node;
      }
    }

    //
    // move item 'guid' one place down
    //
    public function down($guid) {
      $firstnode = null;
      foreach ($this->items() as $node) {
        if ($this->xpath->query("guid", $node)->item(0)->nodeValue == $guid)
          $firstnode = $node;
        else if ($firstnode != null) {
          $node->parentNode->insertBefore($node, $firstnode);
          break;
        }
      }
    }

    //
    // move item 'guid' to top
    //
    public function top($guid) {
      $node = $this->xpath->query("/rss/channel/item/guid[text()='$guid']")->item(0);
      if ($node == NULL)
        return false;
      $node = $node->parentNode;
      $first = $this->xpath->query("/rss/channel/item")->item(0);
      $first->parentNode->insertBefore($node, $first);
      return true;
    }

    //
    // move item 'guid' to bottom
    //
    public function bottom($guid) {
      $node = $this->xpath->query("/rss/channel/item/guid[text()='$guid']")->item(0);
      if ($node == NULL)
        return false;
      $node = $node->parentNode;
      $this->channel->appendChild($node);
      return true;
    }

    //
    // delete item 'guid'
    //
    public function delete($guid) {
      $node = $this->xpath->query("/rss/channel/item/guid[text()='$guid']")->item(0);
      if ($node == NULL)
        return false;
      $node = $node->parentNode;
      while($node->hasChildNodes())
        $node->removeChild($node->childNodes->item(0));
      $node->parentNode->removeChild($node);
      return true;
    }

    //
    // generate uuid
    //
    public function create_uuid() {
      $result = sprintf('%04x%04x-%04x-%04x-%04x-%04x%04x%04x',
        mt_rand(0, 0xFFFF), mt_rand(0, 0xFFFF),
        mt_rand(0, 0xFFFF),
        mt_rand(0, 0x0FFF) | 0x4000,
        mt_rand(0, 0x3FFF) | 0x8000,
        mt_rand(0, 0xFFFF), mt_rand(0, 0xFFFF), mt_rand(0, 0xFFFF)
      );
      return $result;
    }

    //
    // generate guid (deprecated: used in SEM 1.3)
    //
    public function create_guid() {
      return md5(uniqid(rand(), true));
    }

  }

?>
