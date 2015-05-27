package com.example.john.myfirstapp;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.AsyncTask;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import org.json.JSONObject;
import org.w3c.dom.Text;

import java.io.InputStream;
import java.util.Objects;


public class DisplayMessageActivity extends ActionBarActivity {
    public class DownloadImageTask extends AsyncTask<String, Void, Bitmap> {
        ImageView bmImage;

        public DownloadImageTask(ImageView bmImage) {
            this.bmImage = bmImage;
        }

        protected Bitmap doInBackground(String... urls) {
            String urldisplay = urls[0];
            Bitmap mIcon11 = null;
            try {
                InputStream in = new java.net.URL(urldisplay).openStream();
                mIcon11 = BitmapFactory.decodeStream(in);
            } catch (Exception e) {
                Log.e("Error", e.getMessage());
                e.printStackTrace();
            }
            return mIcon11;
        }

        protected void onPostExecute(Bitmap result) {
            Log.i("debug","display image");
            bmImage.setImageBitmap(result);
        }
    }

    Bundle bundle;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display_message);
        Intent intent =this.getIntent();
        bundle = intent.getExtras();
        String json_string = bundle.getString("json");
        final String[] img_url = new String[5];
        final String[] item_url = new String[5];
        int cycle_num;
        final String[] top_rated_item = new String[5];
        final String[] item = new String[5];
        final String[] title = new String[5];
        final String[] price = new String[5];
        final String[] ship_cost = new String[5];
        final String[] item_location = new String[5];
        final String[] categoryName = new String[5];
        final String[] condition = new String[5];
        final String[] buying_format = new String[5];
        final String[] user_name = new String[5];
        final String[] feedback_score = new String[5];
        final String[] positive_feedback = new String[5];
        final String[] facebook_rating = new String[5];
        final String[] top_rating = new String[5];
        final String[] feedback_rating = new String[5];
        final String[] top_rated_seller = new String[5];
        final String[] store = new String[5];
        final String[] shipping_type = new String[5];
        final String[] handling_time = new String[5];
        final String[] shipping_locations = new String[5];
        final String[] expedited_shipping = new String[5];
        final String[] one_day_shipping = new String[5];
        final String[] return_accepted = new String[5];
        //String item[5];
        //int i=0;
        try {
            JSONObject json_obj = new JSONObject(json_string);
            String result_count = json_obj.getString("resultCount");
            if( Integer.parseInt( result_count ) >5){
                cycle_num = 5;
            }
            else{
                cycle_num = Integer.parseInt( result_count );
            }
            Log.v("debug","display message activity");
            for(int i =0; i<cycle_num;i++){
                item[i] = "item"+i;
                JSONObject item_i = json_obj.getJSONObject(item[i]);
                JSONObject BI = item_i.getJSONObject("basicInfo");
                item_url[i] = BI.getString("viewItemURL");
                top_rated_item[i] = BI.getString("topRatedListing");
                title[i] = BI.getString("title");
                item_location[i]= BI.getString("location");
                img_url[i] = BI.getString("pictureURLSuperSize");
                price[i] = BI.getString("convertedCurrentPrice");
                ship_cost[i] = BI.getString("shippingServiceCost");
                categoryName[i] = BI.getString("categoryName");
                condition[i] = BI.getString("conditionDisplayName");if(condition[i].equals("")){condition[i]="N/A";}
                buying_format[i] = BI.getString("listingType");

                JSONObject SI = item_i.getJSONObject("sellerInfo");
                user_name[i] = SI.getString("sellerUserName");
                feedback_score[i] = SI.getString("feedbackScore");
                positive_feedback[i] = SI.getString("positiveFeedbackPercent");
                feedback_rating[i] = SI.getString("feedbackRatingStar");
                top_rated_seller[i] = SI.getString("topRatedSeller");
                store[i] = SI.getString("sellerStoreName"); if(store[i].equals("")){store[i]="N/A";}

                JSONObject SP = item_i.getJSONObject("shippingInfo");
                shipping_type[i] = SP.getString("shippingType");
                handling_time[i] = SP.getString("handlingTime");
                shipping_locations[i] = SP.getString("shipToLocations"); //Log.i("shipping_locations",shipping_locations[i] );

                expedited_shipping[i]= SP.getString("expeditedShipping");
                one_day_shipping[i] = SP.getString("oneDayShippingAvailable");
                return_accepted[i] = SP.getString("returnsAccepted");
                if( ship_cost[i].equals("0.0")|| ship_cost[i].equals("") ){
                    ship_cost[i] = "FREE";
                }
                if(img_url[i].equals("") ){
                    img_url[i] = BI.getString("galleryURL");
                }
            }
        }
        catch(Exception e){}

        //keywords
        String keyword = "Result for '"+bundle.getString("keyword") +"'";
        TextView show_keyword = (TextView) findViewById(R.id.view_keyword);
        show_keyword.setText(keyword);
        //item 0
        final TextView title0 = (TextView) findViewById(R.id.title0);
        title0.setText(title[0]);
        title0.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent0 = new Intent(DisplayMessageActivity.this,View_detail.class);
                Bundle bundle0 =new Bundle();
                bundle0.putString("top_rated_item",top_rated_item[0]);
                bundle0.putString("title",title[0]);
                bundle0.putString("item_url",item_url[0]);
                bundle0.putString("price",price[0]);
                bundle0.putString("ship_cost",ship_cost[0]);
                bundle0.putString("item_location",item_location[0]);
                bundle0.putString("image_url",img_url[0]);


                bundle0.putString("categoryName",categoryName[0]);
                bundle0.putString("condition",condition[0]);
                bundle0.putString("buying_format",buying_format[0]);
                bundle0.putString("user_name",user_name[0]);
                bundle0.putString("feedback_score",feedback_score[0]);
                bundle0.putString("positive_feedback",positive_feedback[0]);
                bundle0.putString("feedback_rating",feedback_rating[0]);
                bundle0.putString("top_rated_seller",top_rated_seller[0]);
                bundle0.putString("store",store[0]);
                //shipping
                bundle0.putString("shipping_type",shipping_type[0]);
                bundle0.putString("handling_time",handling_time[0]);
                bundle0.putString("shipping_loactions",shipping_locations[0]);
                bundle0.putString("expedited_shipping",expedited_shipping[0]);
                bundle0.putString("one_day_shipping",one_day_shipping[0]);
                bundle0.putString("return_accepted",return_accepted[0]);

                intent0.putExtras(bundle0);
                startActivity(intent0);
            }
        });
        TextView show_price = (TextView) findViewById(R.id.price0);
        show_price.setText("Price:$"+price[0]+"("+ship_cost[0]+" Shipping)" );
        ImageView image_view0 = (ImageView) findViewById(R.id.image0 );
        image_view0.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse( item_url[0] ));
                startActivity(browserIntent);
            }
        });
        DownloadImageTask download_img0 = new DownloadImageTask(image_view0);
        download_img0.execute(img_url[0]);
        //item1
        final TextView title1 = (TextView) findViewById(R.id.title1);
        title1.setText(title[1]);
        title1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent1 = new Intent(DisplayMessageActivity.this,View_detail.class);
                Bundle bundle1 =new Bundle();
                bundle1.putString("top_rated_item",top_rated_item[1]);
                bundle1.putString("title",title[1]);
                bundle1.putString("item_url",item_url[1]);
                bundle1.putString("price",price[1]);
                bundle1.putString("ship_cost",ship_cost[1]);
                bundle1.putString("item_location",item_location[1]);
                bundle1.putString("image_url",img_url[1]);


                bundle1.putString("categoryName",categoryName[1]);
                bundle1.putString("condition",condition[1]);
                bundle1.putString("buying_format",buying_format[1]);
                bundle1.putString("user_name",user_name[1]);
                bundle1.putString("feedback_score",feedback_score[1]);
                bundle1.putString("positive_feedback",positive_feedback[1]);
                bundle1.putString("feedback_rating",feedback_rating[1]);
                bundle1.putString("top_rated_seller",top_rated_seller[1]);
                bundle1.putString("store",store[1]);
                //shipping
                bundle1.putString("shipping_type",shipping_type[1]);
                bundle1.putString("handling_time",handling_time[1]);
                bundle1.putString("shipping_loactions",shipping_locations[1]);
                bundle1.putString("expedited_shipping",expedited_shipping[1]);
                bundle1.putString("one_day_shipping",one_day_shipping[1]);
                bundle1.putString("return_accepted",return_accepted[1]);

                intent1.putExtras(bundle1);
                startActivity(intent1);
            }
        });
        TextView show_price1 = (TextView) findViewById(R.id.price1);
        show_price1.setText("Price:$"+price[1]+"("+ship_cost[1]+" Shipping)" );
        ImageView image_view1 = (ImageView) findViewById(R.id.image1 );
        image_view1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse( item_url[1] ));
                startActivity(browserIntent);
            }
        });
        DownloadImageTask download_img1 = new DownloadImageTask(image_view1);
        download_img1.execute(img_url[1]);

        //item 2
        final TextView title2 = (TextView) findViewById(R.id.title2);
        title2.setText(title[2]);
        title2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent2 = new Intent(DisplayMessageActivity.this,View_detail.class);
                Bundle bundle2 =new Bundle();
                bundle2.putString("top_rated_item",top_rated_item[2]);
                bundle2.putString("title",title[2]);
                bundle2.putString("item_url",item_url[2]);
                bundle2.putString("price",price[2]);
                bundle2.putString("ship_cost",ship_cost[2]);
                bundle2.putString("item_location",item_location[2]);
                bundle2.putString("image_url",img_url[2]);

                bundle2.putString("categoryName",categoryName[2]);
                bundle2.putString("condition",condition[2]);
                bundle2.putString("buying_format",buying_format[2]);
                bundle2.putString("user_name",user_name[2]);
                bundle2.putString("feedback_score",feedback_score[2]);
                bundle2.putString("positive_feedback",positive_feedback[2]);
                bundle2.putString("feedback_rating",feedback_rating[2]);
                bundle2.putString("top_rated_seller",top_rated_seller[2]);
                bundle2.putString("store",store[2]);
                //shipping
                bundle2.putString("shipping_type",shipping_type[2]);
                bundle2.putString("handling_time",handling_time[2]);
                bundle2.putString("shipping_loactions",shipping_locations[2]);
                bundle2.putString("expedited_shipping",expedited_shipping[2]);
                bundle2.putString("one_day_shipping",one_day_shipping[2]);
                bundle2.putString("return_accepted",return_accepted[2]);

                intent2.putExtras(bundle2);
                startActivity(intent2);
            }
        });
        TextView show_price2 = (TextView) findViewById(R.id.price2);
        show_price2.setText("Price:$"+price[2]+"("+ship_cost[2]+" Shipping)" );
        ImageView image_view2 = (ImageView) findViewById(R.id.image2 );
        image_view2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse( item_url[2] ));
                startActivity(browserIntent);
            }
        });
        DownloadImageTask download_img2 = new DownloadImageTask(image_view2);
        download_img2.execute(img_url[2]);

        //item3
        final TextView title3 = (TextView) findViewById(R.id.title3);
        title3.setText(title[3]);
        title3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent3 = new Intent(DisplayMessageActivity.this,View_detail.class);
                Bundle bundle3 =new Bundle();
                bundle3.putString("top_rated_item",top_rated_item[3]);
                bundle3.putString("title",title[3]);
                bundle3.putString("item_url",item_url[3]);
                bundle3.putString("price",price[3]);
                bundle3.putString("ship_cost",ship_cost[3]);
                bundle3.putString("item_location",item_location[3]);
                bundle3.putString("image_url",img_url[3]);

                bundle3.putString("categoryName",categoryName[3]);
                bundle3.putString("condition",condition[3]);
                bundle3.putString("buying_format",buying_format[3]);
                bundle3.putString("user_name",user_name[3]);
                bundle3.putString("feedback_score",feedback_score[3]);
                bundle3.putString("positive_feedback",positive_feedback[3]);
                bundle3.putString("feedback_rating",feedback_rating[3]);
                bundle3.putString("top_rated_seller",top_rated_seller[3]);
                bundle3.putString("store",store[3]);
                //shipping
                bundle3.putString("shipping_type",shipping_type[3]);
                bundle3.putString("handling_time",handling_time[3]);
                bundle3.putString("shipping_loactions",shipping_locations[3]);
                bundle3.putString("expedited_shipping",expedited_shipping[3]);
                bundle3.putString("one_day_shipping",one_day_shipping[3]);
                bundle3.putString("return_accepted",return_accepted[3]);

                intent3.putExtras(bundle3);
                startActivity(intent3);
            }
        });
        TextView show_price3 = (TextView) findViewById(R.id.price3);
        show_price3.setText("Price:$"+price[3]+"("+ship_cost[3]+" Shipping)" );
        ImageView image_view3 = (ImageView) findViewById(R.id.image3 );
        image_view3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse( item_url[3] ));
                startActivity(browserIntent);
            }
        });
        DownloadImageTask download_img3 = new DownloadImageTask(image_view3);
        download_img3.execute(img_url[3]);

        //item4
        final TextView title4 = (TextView) findViewById(R.id.title4);
        title4.setText(title[4]);
        title4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent4 = new Intent(DisplayMessageActivity.this,View_detail.class);
                Bundle bundle4 =new Bundle();
                bundle4.putString("top_rated_item",top_rated_item[4]);
                bundle4.putString("title",title[4]);
                bundle4.putString("item_url",item_url[4]);
                bundle4.putString("price",price[4]);
                bundle4.putString("ship_cost",ship_cost[4]);
                bundle4.putString("item_location",item_location[4]);
                bundle4.putString("image_url",img_url[4]);


                bundle4.putString("categoryName",categoryName[4]);
                bundle4.putString("condition",condition[4]);
                bundle4.putString("buying_format",buying_format[4]);
                bundle4.putString("user_name",user_name[4]);
                bundle4.putString("feedback_score",feedback_score[4]);
                bundle4.putString("positive_feedback",positive_feedback[4]);
                bundle4.putString("feedback_rating",feedback_rating[4]);
                bundle4.putString("top_rated_seller",top_rated_seller[4]);
                bundle4.putString("store",store[4]);
                //shipping
                bundle4.putString("shipping_type",shipping_type[4]);
                bundle4.putString("handling_time",handling_time[4]);
                bundle4.putString("shipping_loactions",shipping_locations[4]);
                bundle4.putString("expedited_shipping",expedited_shipping[4]);
                bundle4.putString("one_day_shipping",one_day_shipping[4]);
                bundle4.putString("return_accepted",return_accepted[4]);

                intent4.putExtras(bundle4);
                startActivity(intent4);
            }
        });
        TextView show_price4 = (TextView) findViewById(R.id.price4);
        show_price4.setText("Price:$"+price[4]+"("+ship_cost[4]+" Shipping)" );
        ImageView image_view4 = (ImageView) findViewById(R.id.image4 );
        image_view4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse( item_url[4] ));
                startActivity(browserIntent);
            }
        });
        DownloadImageTask download_img4 = new DownloadImageTask(image_view4);
        download_img4.execute(img_url[4]);
/*
        final TextView title1 = (TextView) findViewById(R.id.title1);
        title1.setText(title[1]);
        title1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent1 = new Intent(DisplayMessageActivity.this,View_detail.class);
                Bundle bundle1 =new Bundle();
                bundle1.putString("title",title[1]);
                intent1.putExtras(bundle1);
                startActivity(intent1);
            }
        });
        TextView show_price1 = (TextView) findViewById(R.id.price1);
        show_price1.setText("Price:$"+price[1]+"("+ship_cost[1]+" Shipping)" );
        ImageView image_view1 = (ImageView) findViewById(R.id.image1 );
        image_view1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse( item_url[1] ));
                startActivity(browserIntent);
            }
        });
        DownloadImageTask download_img1 = new DownloadImageTask(image_view1);
        download_img1.execute(img_url[1]);
*/
}

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
