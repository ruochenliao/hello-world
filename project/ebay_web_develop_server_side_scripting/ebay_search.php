<HTML><HEAD>
<meta name="viewport" content="height=device-height">
<style type = "text/css">
td.groove{
	border-bottom-style: groove;
}
td.remove{
	border-bottom-style:none;
}
</style>
<script>
	
function validation(){
	var x =1;
	var my_keywords = document.getElementsByName("Keywords")[0].value;
	var minPrice = Number( document.getElementsByName("MinPrice")[0].value);
	var maxPrice = Number( document.getElementsByName("MaxPrice")[0].value);
	var MaxHandlingTime = document.getElementsByName("MaxHandlingTime")[0].value;
	if( my_keywords ==""){
		alert("Please enter value for Key Words\n");
		x =0;
	}
	if( (minPrice <0) ||(maxPrice<0)||(  (minPrice > maxPrice)&&( (minPrice !="") && (maxPrice != "") )  )){
		alert("price range input error!");
		x =0;
	}
	if( isNaN(MaxHandlingTime)|| MaxHandlingTime <1){
		if( MaxHandlingTime !="" ){
			alert("Max handling time input format error: MaxhandlingTime should > 1");
			x =0;
		}
	}
	if(x ==0 ){
		document.getElementById("test").value = x;
		}
	else{
		document.getElementById("test").value = x;
	}
}

function clear_function(){
//	document.forms[0].reset();
	document.forms[0].Keywords.value ="";
	document.forms[0].MinPrice.value ="";
	document.forms[0].MaxPrice.value ="";
	document.forms[0].MaxHandlingTime.value ="";
	document.getElementById("condition0").checked =false;
	document.getElementById("condition1").checked =false;
	document.getElementById("condition2").checked =false;
	document.getElementById("condition3").checked =false;
	document.getElementById("condition4").checked =false;
	
	document.getElementById("ListingType0").checked =false;
	document.getElementById("ListingType1").checked =false;
	document.getElementById("ListingType2").checked =false;
	
	document.getElementById("ReturnsAcceptedOnly").checked =false;
	
	document.getElementById("FreeShippingOnly").checked =false;
	document.getElementById("ExpeditedShippingType").checked =false;
	
	//option
	document.getElementById("CurrentPriceHighest").selected =false;
	//document.getElementById("CurrentPriceLowest").selected =false;
	document.getElementById("PricePlusShippingHighest").selected =false;
	document.getElementById("PricePlusShippingLowest").selected =false;
	
	//option
	//document.getElementById("page0").selected =false;
	document.getElementById("page1").selected =false;
	document.getElementById("page2").selected =false;
	document.getElementById("page3").selected =false;
	document.getElementById("div_clear").innerHTML = "";
}
</script>
<TITLE>
</TITLE></HEAD><BODY>
<fieldset style ="border-style:solid;border-width:3px;border-color:black;">
<center><table><tr><td class= "remove"><image src ="ebay.jpg" height ="100" width ="100"></td><td class= "remove"><span style="font-size:300%; top:20px"> shopping</span></td></tr></table></center>
<fieldset style ="border-style:solid;border-width:5px; border-color:black;"><table style="width:100%"><FORM id="myform" name="myform" METHOD="GET">
<tr><td class="groove"><b>Key Words*:</b></td>  <td class="groove"><input type ="text" name ="Keywords" id ="Keywords" style="width:100%" value ="<?php if(isset($_GET["Keywords"])){echo $_GET["Keywords"];}?>"></td></tr>
<tr><td class="groove"><b>Price Range:</b></td>  <td class="groove">from $<input type ="text" name ="MinPrice" id="MinPrice" value ="<?php if(isset($_GET["MinPrice"])){echo $_GET["MinPrice"];}?>" > to $<input type ="text" name ="MaxPrice" id = "MaxPrice" value ="<?php if(isset($_GET["MaxPrice"])){echo $_GET["MaxPrice"];}?>" ></td></tr>

<tr><td class="groove"><b>Condition:</b></td><td class="groove">
		<input type ="checkbox" name ="Condition[]" id="condition0" value ="1000" <?php if(isset( $_GET["Condition"])){for($ii=0;$ii<count( $_GET["Condition"] )  ;$ii++ ){  if ($_GET["Condition"][$ii]== "1000") echo 'checked'; }} ?> >New 
		<input type ="checkbox" name ="Condition[]" id="condition1" value ="3000" <?php if(isset( $_GET["Condition"])){for($ii=0;$ii<count( $_GET["Condition"] )  ;$ii++ ){  if ($_GET["Condition"][$ii]== "3000") echo 'checked'; }} ?> >Used 
		<input type ="checkbox" name ="Condition[]" id="condition2" value ="4000" <?php if(isset( $_GET["Condition"])){for($ii=0;$ii<count( $_GET["Condition"] )  ;$ii++ ){  if ($_GET["Condition"][$ii]== "4000") echo 'checked'; }} ?> >Very Good 
		<input type ="checkbox" name ="Condition[]" id="condition3" value ="5000" <?php if(isset( $_GET["Condition"])){for($ii=0;$ii<count( $_GET["Condition"] )  ;$ii++ ){  if ($_GET["Condition"][$ii]== "5000") echo 'checked'; }} ?> >Good
		<input type ="checkbox" name ="Condition[]" id="condition4" value ="6000" <?php if(isset( $_GET["Condition"])){for($ii=0;$ii<count( $_GET["Condition"] )  ;$ii++ ){  if ($_GET["Condition"][$ii]== "6000") echo 'checked'; }} ?> >Acceptable</td></tr> 
<tr><td class="groove"><b>Buying formats:</b></td>
	<td class="groove">
		<input type ="checkbox" name ="ListingType[]" id ="ListingType0" value ="FixedPrice" <?php if(isset( $_GET["ListingType"])){for($jj=0;$jj<count( $_GET["ListingType"] )  ;$jj++ ){  if ($_GET["ListingType"][$jj]== "FixedPrice") echo 'checked'; }} ?> >Buy It Now
		<input type ="checkbox" name ="ListingType[]" id ="ListingType1" value ="Auction"    <?php if(isset( $_GET["ListingType"])){for($jj=0;$jj<count( $_GET["ListingType"] )  ;$jj++ ){  if ($_GET["ListingType"][$jj]== "Auction") echo 'checked'; }} ?> >Auction 
		<input type ="checkbox" name ="ListingType[]" id ="ListingType2" value ="Classified" <?php if(isset( $_GET["ListingType"])){for($jj=0;$jj<count( $_GET["ListingType"] )  ;$jj++ ){  if ($_GET["ListingType"][$jj]== "Classified") echo 'checked'; }} ?> >Classified Ads</td></tr>
<tr><td class="groove"><b>Seller:</b></td>
	<td class="groove"><input type ="checkbox" name ="ReturnsAcceptedOnly" id = "ReturnsAcceptedOnly" value = "true" <?php if(isset($_GET["ReturnsAcceptedOnly"] )){ $array_c=$_GET["ReturnsAcceptedOnly"]; echo 'checked = "$array_c[0]"';} ?> >Return accepted</td></tr>
<tr><td rowspan = 3 class="groove"><b>Shipping</b></td><td ><input type ="checkbox" name ="FreeShippingOnly" id = "FreeShippingOnly" value ="true" <?php if(isset($_GET["FreeShippingOnly"] )){ $array_d=$_GET["FreeShippingOnly"]; echo 'checked = "$array_d[0]"';} ?> >Free Shipping</td></tr>
	<tr><td><input type ="checkbox" name ="ExpeditedShippingType" id ="ExpeditedShippingType" value = "Expedited" <?php if(isset($_GET["ExpeditedShippingType"] )){ $array_d=$_GET["ExpeditedShippingType"]; echo 'checked = "$array_e[0]"';} ?> >Expedited shipping available</td></tr>
	<tr><td class = "groove">Max handling time(days):<input type ="text" name ="MaxHandlingTime" id ="MaxHandlingTime" value =<?php if(isset($_GET["MaxHandlingTime"]))echo $_GET["MaxHandlingTime"];?>></td></tr>
<tr><td class="groove"><b>Sort by:</b></td>
	<td class="groove"><SELECT name="sortOrder">
		<OPTION value ="BestMatch" id ="BestMatch" > Best Match 
		<OPTION value ="CurrentPriceHighest" id ="CurrentPriceHighest" <?php if(isset($_GET["sortOrder"])){ if ($_GET["sortOrder"] =="CurrentPriceHighest" ){ echo 'selected';} }?>  > Price:highest first 
		
		<OPTION value="PricePlusShippingHighest" id = "PricePlusShippingHighest" <?php if(isset($_GET["sortOrder"])){ if ($_GET["sortOrder"] =="PricePlusShippingHighest" ){ echo 'selected';} }?> > Price + shipping: highest first 
		<OPTION value="PricePlusShippingLowest" id ="PricePlusShippingLowest" <?php if(isset($_GET["sortOrder"])){ if ($_GET["sortOrder"] =="PricePlusShippingLowest" ){ echo 'selected';} }?> > Price + shipping: lowest first</SELECT></td></tr>
<tr><td><b>Results Per Page:</b></td>
	<td><SELECT name="paginationInput">
		<OPTION value =5  id ="page0" <?php if(isset($_GET["paginationInput"])){ if ($_GET["paginationInput"] ==5 ){ echo 'selected';} }?> > 5 
		<OPTION value =10 id ="page1" <?php if(isset($_GET["paginationInput"])){ if ($_GET["paginationInput"] ==10 ){ echo 'selected';} }?> > 10 
		<OPTION value =15 id ="page2" <?php if(isset($_GET["paginationInput"])){ if ($_GET["paginationInput"] ==15 ){ echo 'selected';} }?> > 15
		<OPTION value =20 id ="page3" <?php if(isset($_GET["paginationInput"])){ if ($_GET["paginationInput"] ==20 ){ echo 'selected';} }?> > 20 </SELECT></td></tr><br/>
<input type = "hidden" id ="test" name = "test">		
		
<tr><td colspan = 2 style="text-align:right"><input type ="button" value ="clear" onclick ="clear_function()" ><INPUT type=submit name="submit" value = "search" onclick ="validation()"></td></tr>

</FORM></table></fieldset>

<?php if( isset($_GET["submit"]) && $_GET['test']): 
//<OPTION value="CurrentPriceLowest" id ="CurrentPriceLowest" <php if(isset($_GET["sortOrder"])){ if ($_GET["sortOrder"] =="CurrentPriceLowest" ){ echo 'selected';} }> > Price:lowest first
	//echo $_GET["Keywords"]."<br>";
//<HTML><HEAD><TITLE></TITLE></HEAD><BODY><pre>
//	$url = "http://svcs.eBay.com/services/search/FindingService/v1?siteid=0&SECURITYAPPNAME=johnliao-84fa-4511-bc6e-6ad39e9bf42b&OPERATION-NAME=findItemsAdvanced&SERVICEVERSION=1.0.0&RESPONSE-DATA-FORMAT=XML";
	$url = "http://svcs.ebay.com/services/search/FindingService/v1?siteid=0&SECURITY-APPNAME=johnliao-84fa-4511-bc6e-6ad39e9bf42b&OPERATION-NAME=findItemsAdvanced&SERVICE-VERSION=1.0.0&RESPONSE-DATA-FORMAT=XML";
	$keyWord = $_GET["Keywords"];
	$keyWord = urlencode($keyWord);
	$sortby = $_GET["sortOrder"];
	$page = (int)$_GET["paginationInput"];
	$minPrice = (int)$_GET["MinPrice"];
	$maxPrice = (int)$_GET["MaxPrice"];
	$url=$url."&keywords=$keyWord"."&sortOrder=$sortby"."&paginationInput.entriesPerPage=$page";
//	$url .= "&itemFilter(0).name=MinPrice&itemFilter(0).value=$minPrice";
	$i =0;
//$url = $url."&itemFilter(0).name=keywords"."&itemFilter(0).value=$keyWord";
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
		$url.="&itemFilter($i).name=ReturnsAcceptedOnly&itemFilter($i).value=$returnAccept";
		$i++;
	}
	if( isset($_GET["FreeShippingOnly"]) ){
		$Freeshipping = $_GET["FreeShippingOnly"];
		$url.="&itemFilter($i).name=FreeShippingOnly&itemFilter($i).value=$Freeshipping";
		$i++;
	}	
	if( isset($_GET["ExpeditedShippingType"]) ){
		$ExpeditedShipping = $_GET["ExpeditedShippingType"];
		$url.="&itemFilter($i).name=ExpeditedShippingType&itemFilter($i).value=$ExpeditedShipping";
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
	$xml = simplexml_load_file($url);
	$totalEntries = $xml -> paginationOutput->totalEntries;
	$html_text = '<div id = "div_clear">';
	if( $totalEntries ==0 ){
		$noResult = "No result found";
		$html_text .= '<center><font size ="7">'.$noResult."</font></center><br>";
		//echo '<center><font size ="7">'.$noResult."</font></center><br>";
	}
	else{
		//echo $page;
		if($totalEntries <$page){
			$page = $totalEntries;
		}
		//$html_text = '<fieldset style = "border-style:solid;border-width:5px; border-color:black;">';
		//$html_text = '<div id = "div_clear">';
		$html_text .= '<center><font size ="5">'.$totalEntries." result for ".urldecode($keyWord)."</font></center>"."<br>";
		$html_text .= '<fieldset style = "padding:0">';
		$html_text .= '<table border = 0px style="width:100%">';
		//$page
		for( $j=0; $j<$page;$j++ ){
			$imageURL = $xml->searchResult -> item[$j] -> galleryURL;
			$itemDescription = $xml ->searchResult->item[$j]->title;
			$item_link_description = $xml -> searchResult->item[$j]-> viewItemURL;
			$condition_display = $xml ->searchResult->item[$j]->condition->conditionDisplayName;
	

			$buyFormt = $xml -> searchResult->item[$j]->listingInfo->listingType;
			$acceptReturn = $xml -> searchResult->item[$j]->returnsAccepted;
			$freeShipping = $xml-> searchResult->item[$j]->shippingInfo-> shippingServiceCost;
			$expeditedShipping = $xml -> searchResult->item[$j]->shippingInfo->expeditedShipping;
			$handlingTime = $xml -> searchResult->item[$j]->shippingInfo->handlingTime;
			$priceValue = $xml -> searchResult->item[$j]->sellingStatus-> currentPrice;
			$shippingCost = $xml -> searchResult->item[$j]->shippingInfo->shippingServiceCost;
			$shippingLocation = $xml -> searchResult->item[$j]->location;
			//style ="max-height: 100%"
			$html_text .='<tr><td rowspan = 7 style =" border-bottom-style: inset;border-bottom-width: 1px;">'.'<img style ="height:230px; width:200px;" src= "'.$imageURL.'"></td></tr>';		                  
			$html_text .="<tr><td><br><br><a href = ".$item_link_description.">".$itemDescription."</a></td></tr>"; 
			if( ($xml -> searchResult->item[$j]->topRatedListing) == "true"  ){
				$html_text .='<tr><td>'."<b>Condition:</b>".$condition_display;
				$html_text .= '<img style ="height:5%; width:5%" src ="http://cs-server.usc.edu:45678/hw/hw6/itemTopRated.jpg"></td></tr>';
			}
			else{															
				$html_text .="<tr><td>"."<b>Condition:</b>".$condition_display."<br><br></td></tr>";
				//$html_text .= "<br><br>";
			}
			if( ($buyFormt =="FixedPrice")|| ($buyFormt =="StoreInventory") )
				$html_text .="<tr><td><b>"."Buy It Now"."</b><br><br></td></tr>";
			else if( $buyFormt == "Auction")
				$html_text .="<tr><td><b>"."Auction"."</b><br><br></td></tr>";
			else
				$html_text .="<tr><td><b>"."Classified Ad"."</b><br><br></td></tr>";
				//$html_text .="<tr><td><b>"."Auction"."</b><br><br></td></tr>";		
		
			if($acceptReturn == "true")
				$html_text .="<tr><td>Seller Accept return</td></tr>";
			else 
				$html_text .="<tr><td>Seller Refused return</td></tr>";
			if($freeShipping == "0.0")
				$html_text .="<tr><td>Shipping Free --";
			else 
				$html_text .="<tr><td>Shipping Not Free -- ";
			if($expeditedShipping =="true")
				$html_text .= "Expedited Shipping Available -- ";
			else 
				$html_text .= "Expedited Shipping Not Available -- ";
			$html_text .= "Handled for shipping in ".$handlingTime."day(s)<br><br></td></tr>";
			$html_text .='<tr><td style =" border-bottom-style: inset;border-bottom-width: 1px;"><b>Price:$'.$priceValue." ";
			if($shippingCost)
				$html_text .="(+$".$shippingCost." for shipping)</b> ";
			$html_text .="From ".$shippingLocation."</td></tr>";	
		}
		$html_text .= "</table></fieldset>";
		$html_text .= "</div>";
		//echo $html_text;
	}
	//echo $url;
	echo $html_text;
	//print_r($xml);
?>
<?php endif; ?>


</fieldset>
</BODY>
</HTML>