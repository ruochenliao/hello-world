<?php
/*
	if(isset($_GET["keywords"]) ){
		//echo $_GET["keywords"];
		$return = $_GET;
		$return["json"]=json_encode($return);
		echo json_encode($return);
	}
	*/
	
	$url = "http://svcs.ebay.com/services/search/FindingService/v1?siteid=0&SECURITY-APPNAME=johnliao-84fa-4511-bc6e-6ad39e9bf42b&OPERATION-NAME=findItemsAdvanced&SERVICE-VERSION=1.0.0&RESPONSE-DATA-FORMAT=XML";
	$keyWord = $_GET["Keywords"];
	$keyWord = urlencode($keyWord);
	$sortby = $_GET["sortOrder"];
	$page = (int)$_GET["paginationInput"];
	$minPrice = (int)$_GET["MinPrice"];
	$maxPrice = (int)$_GET["MaxPrice"];
	$url=$url."&keywords=$keyWord"."&sortOrder=$sortby"."&paginationInput.entriesPerPage=$page";
	$i =0;
	if( $minPrice != "" ){
		$url .= "&itemFilter($i).name=MinPrice&itemFilter($i).value=$minPrice";
		$i++;
	}
	if( $maxPrice != "" ){
		$url.="&itemFilter($i).name=MaxPrice&itemFilter($i).value=$maxPrice";
		$i++;
	}
	if( isset($_GET["Condition"]) ){
		$array = $_GET["Condition"];
		foreach($array as $key => $value){
			$url.="&itemFilter($i).name=Condition&itemFilter($i).value[$key]=$value";
		}
		$i++;
	}
	if( isset($_GET["ListingType"]) ){
		foreach($_GET["ListingType"] as $key => $value){
			$url.="&itemFilter($i).name=ListingType&itemFilter($i).value[$key]=$value";
		}
		$i++;
	}
	
	if( isset($_GET["ReturnsAcceptedOnly"]) ){
		$returnAccept= $_GET["ReturnsAcceptedOnly"];
		//$url.="&itemFilter($i).name=ReturnsAcceptedOnly&itemFilter($i).value=$returnAccept";
		$url.="&itemFilter($i).name=ReturnsAcceptedOnly&itemFilter($i).value=".$returnAccept[0];
		$i++;
	}
	if( isset($_GET["FreeShippingOnly"]) ){
		$Freeshipping = $_GET["FreeShippingOnly"];
		//$url.="&itemFilter($i).name=FreeShippingOnly&itemFilter($i).value=$Freeshipping";
		$url.="&itemFilter($i).name=FreeShippingOnly&itemFilter($i).value=".$Freeshipping[0];
		$i++;
	}	
	if( isset($_GET["ExpeditedShippingType"]) ){
		$ExpeditedShipping = $_GET["ExpeditedShippingType"];
		//$url.="&itemFilter($i).name=ExpeditedShippingType&itemFilter($i).value=$ExpeditedShipping";
		$url.="&itemFilter($i).name=ExpeditedShippingType&itemFilter($i).value=".$ExpeditedShipping[0];
		$i++;
	}	
	if( isset($_GET["MaxHandlingTime"]) =="true" ){
		$MaxHandlingTime = $_GET["MaxHandlingTime"];
		if( $MaxHandlingTime !="" ){
			$MaxHandlingTime = (int)$_GET["MaxHandlingTime"];
			$url.="&itemFilter($i).name=MaxHandlingTime&itemFilter($i).value=$MaxHandlingTime";
			$i++;
		}
	}
	
	//$xml = simplexml_load_file($url);
	
	$url.="&outputSelector[0]=SellerInfo";
	$url.="&outputSelector[1]=PictureURLSuperSize";
	$url.="&outputSelector[2]=StoreInfo";
    $page_x=$_GET["pageNumber"];
    $url.="&paginationInput.pageNumber=$page_x";
    //echo url;
    $xml = simplexml_load_file($url);
	$ack=$xml->ack;
	$totalEntries=$xml->paginationOutput->totalEntries;
	$pageNumber=$xml->paginationOutput->pageNumber;
	$entriesPerPage=$xml->paginationOutput->entriesPerPage;
/*	
	$page = (int)$_GET["paginationInput"];
	$url.="&paginationInput.pageNumber=".$page;
*/
	$array_json =[ 
		"ack" => "$ack",
		"resultCount" => "$totalEntries",
		"pageNumber"=> "$pageNumber", 
		"itemCount" => "$entriesPerPage",
	];
	//echo $page;
	//echo json_encode($array_json);
	if($totalEntries ==0){
		$json_return='{"ack":"fail"}';
		echo $json_return;
	}
	else{
        $itemlength = $xml->searchResult->item->count();
        //echo $itemlength;
		//for($i=0; $i <$page ; $i++){
        for($i=0; $i <$itemlength; $i++){
            $pictureURLSuperSize=$xml->searchResult->item[$i]->pictureURLSuperSize;
			$title=$xml->searchResult->item[$i]->title;
			$viewItemURL=$xml->searchResult->item[$i]->viewItemURL;
			$galleryURL=$xml->searchResult->item[$i]->galleryURL;
			$convertedCurrentPrice=$xml->searchResult->item[$i]->sellingStatus->convertedCurrentPrice;
			$shippingServiceCost=$xml->searchResult->item[$i]->shippingInfo->shippingServiceCost;
			$conditionDisplayName=$xml->searchResult->item[$i]->condition->conditionDisplayName;
			$listingType=$xml->searchResult->item[$i]->listingInfo->listingType;
			$location=$xml->searchResult->item[$i]->location;
			$categoryName=$xml->searchResult->item[$i]->primaryCategory->categoryName;
			$topRatedListing=$xml->searchResult->item[$i]->topRatedListing;	
			
			$sellerUserName=$xml->searchResult->item[$i]->sellerInfo->sellerUserName;
			$feedbackScore=$xml->searchResult->item[$i]->sellerInfo->feedbackScore;
			$positiveFeedbackPercent=$xml->searchResult->item[$i]->sellerInfo->positiveFeedbackPercent;
			$feedbackRatingStar=$xml->searchResult->item[$i]->sellerInfo->feedbackRatingStar;
			$topRatedSeller=$xml->searchResult->item[$i]->sellerInfo->topRatedSeller;
			$storeName=$xml->searchResult->item[$i]->storeInfo->storeName;
			$storeURL=$xml->searchResult->item[$i]->storeInfo->storeURL;
			
			$shippingType=$xml->searchResult->item[$i]->shippingInfo->shippingType;
			$shipToLocations=$xml->searchResult->item[$i]->shippingInfo->shipToLocations;
			$expeditedShipping=$xml->searchResult->item[$i]->shippingInfo->expeditedShipping;
			$oneDayShippingAvailable=$xml->searchResult->item[$i]->shippingInfo->oneDayShippingAvailable;
			$returnsAccepted=$xml->searchResult->item[$i]->returnsAccepted;
			$handlingTime=$xml->searchResult->item[$i]->shippingInfo->handlingTime;
			
			$basicInfo=array(
                "pictureURLSuperSize"=>"$pictureURLSuperSize",
				"title"=>"$title",
				"viewItemURL" =>"$viewItemURL",
				"galleryURL" =>"$galleryURL",
				"convertedCurrentPrice"=>"$convertedCurrentPrice",
				"shippingServiceCost"=>"$shippingServiceCost",
				"conditionDisplayName"=>"$conditionDisplayName",
				"listingType"=>"$listingType",
				"location"=>"$location",
				"categoryName"=>"$categoryName",
				"topRatedListing"=>"$topRatedListing",
			);
			$sellerInfo=array(
				"sellerUserName"=>"$sellerUserName",
				"feedbackScore"=>"$feedbackScore",
				"positiveFeedbackPercent"=>"$positiveFeedbackPercent",
				"feedbackRatingStar"=>"$feedbackRatingStar",
				"topRatedSeller"=>"$topRatedSeller",
				"sellerStoreName"=>"$storeName",
				"sellerStoreURL"=>"$storeURL",
			);
			$shippingInfo=array(
				"shippingType"=>"$shippingType",
				"shipToLocations"=>"$shipToLocations",
				"expeditedShipping"=>"$expeditedShipping",
				"oneDayShippingAvailable"=>"$oneDayShippingAvailable",
				"returnsAccepted"=>"$returnsAccepted",
				"handlingTime"=>"$handlingTime",
			);
			$item[$i]["basicInfo"] =$basicInfo;
			$item[$i]["sellerInfo"] =$sellerInfo;
			$item[$i]["shippingInfo"] =$shippingInfo;
			/*
			$item[$i]=["basicInfo"=> "$basicInfo"];
			$item[$i]=["sellerInfo"=>"$sellerInfo"];
			$item[$i]=["shippingInfo"=>"$shippingInfo"];
			*/
			//$array_json =[ "item".$i=> "$item[".$i."]" ];
			$array_json["item".$i]=$item[$i] ;
		}
		echo json_encode($array_json);
	}
?>