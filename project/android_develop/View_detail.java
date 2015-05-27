package com.example.john.myfirstapp;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.net.Uri;
import android.os.AsyncTask;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.facebook.AccessToken;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.FacebookSdk;
import com.facebook.Profile;
import com.facebook.login.LoginManager;
import com.facebook.share.Sharer;
import com.facebook.share.model.ShareLinkContent;
import com.facebook.share.model.SharePhoto;
import com.facebook.share.model.SharePhotoContent;
import com.facebook.share.widget.ShareButton;
import com.facebook.share.widget.ShareDialog;

import org.w3c.dom.Text;

import java.io.InputStream;
import java.util.Arrays;
import java.util.List;


public class View_detail extends ActionBarActivity {
    CallbackManager callbackManager;
    ShareDialog shareDialog;

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
            //Log.i("debug","display image");
            bmImage.setImageBitmap(result);
        }
    }

    @Override
    protected void onActivityResult(final int requestCode, final int resultCode, final Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        callbackManager.onActivityResult(requestCode, resultCode, data);
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        FacebookSdk.sdkInitialize(getApplicationContext());
        callbackManager = CallbackManager.Factory.create();
        shareDialog = new ShareDialog(this);

        shareDialog.registerCallback(callbackManager, new FacebookCallback<Sharer.Result>() {
                    @Override
                    public void onSuccess(Sharer.Result result) {
                        Log.i("debug","facebook_inside");
                        String post_id= result.getPostId();
                        //Log.i("post_id", post_id);

                        if(post_id ==null) {
                            Toast.makeText(View_detail.this, "post is canceled", Toast.LENGTH_SHORT).show();
                        }
                        else{
                            Toast.makeText(View_detail.this, "Facebook Post Successful", Toast.LENGTH_SHORT).show();
                        }
                    }
                    @Override
                    public void onCancel() {
                        Log.i("facebook", "cancled");
                        Toast.makeText(View_detail.this, "Share Cancelled", Toast.LENGTH_SHORT).show();
                    }
                    @Override
                    public void onError(FacebookException exception) {
                        Toast.makeText(View_detail.this, exception.getMessage(), Toast.LENGTH_LONG).show();
                        //Log.e(TAG, "Share: " + exception.getMessage());
                        exception.printStackTrace();
                    }
        });

        setContentView(R.layout.activity_view_detail);
        Bundle bundle;
        Intent intent =this.getIntent();
        bundle = intent.getExtras();
        final String title = bundle.getString("title");
        final String image_url = bundle.getString("image_url");
        final String price = bundle.getString("price");
        final String ship_cost = bundle.getString("ship_cost");
        final String item_location = bundle.getString("item_location");
        final String item_url = bundle.getString("item_url"); Log.i("item_url",item_url);
        String top_rated_item = bundle.getString("top_rated_item");
        //basic info
        String categoryName = bundle.getString("categoryName");Log.i("hello",categoryName);
        String condition = bundle.getString("condition");Log.i("hello",condition);
        String buying_format = bundle.getString("buying_format");Log.i("hello",buying_format);

        TextView categoryName_s = (TextView)findViewById(R.id.category_name_s);
        categoryName_s.setText(categoryName);
        TextView condition_s = (TextView)findViewById(R.id.condition_s);
        condition_s.setText(condition);
        TextView buying_format_s = (TextView)findViewById(R.id.buying_format_s);
        buying_format_s.setText(buying_format);
        //seller
        String user_name = bundle.getString("user_name");Log.i("hello",user_name);
        String feedback_score = bundle.getString("feedback_score");Log.i("hello",feedback_score);
        String positive_feedback = bundle.getString("positive_feedback");Log.i("hello",positive_feedback);
        String feedback_rating = bundle.getString("feedback_rating");Log.i("hello",feedback_rating);
        String top_rated_seller = bundle.getString("top_rated_seller");Log.i("hello",top_rated_seller);
        String store = bundle.getString("store");

        TextView user_name_s = (TextView)findViewById(R.id.user_name_s);
        user_name_s.setText(user_name);
        TextView feedback_score_s = (TextView)findViewById(R.id.feedback_score_s);
        feedback_score_s.setText(feedback_score);
        TextView positive_feedback_s = (TextView)findViewById(R.id.positive_feedback_s);
        positive_feedback_s.setText(positive_feedback);
        TextView feedback_rating_s = (TextView)findViewById(R.id.feedback_rating_s);
        feedback_rating_s.setText(feedback_rating);
        TextView store_s = (TextView)findViewById(R.id.store_s);
        store_s.setText(store);
        ImageView top_rated_seller_s = (ImageView)findViewById(R.id.top_rated_s);
        if(top_rated_seller.equals("true") ){
            top_rated_seller_s.setImageResource(R.drawable.gou);
        }
        else{
            top_rated_seller_s.setImageResource(R.drawable.chacha);
        }

        //shipping
        String shipping_type = bundle.getString("shipping_type");Log.i("hello",shipping_type);
        String handling_time = bundle.getString("handling_time");Log.i("hello",handling_time);
        String shipping_loaction = bundle.getString("shipping_loactions");Log.i("hello",shipping_loaction);
        /*
        Log.i("shipping_locations",String.valueOf(shipping_loaction.length()) );
        int location_length;
        String location_string = "";
        for(location_length=0;location_length<shipping_loaction.length();location_length++ ){
            location_string=location_string+shipping_loaction[location_length];
        }
        */
        String expedited_shipping = bundle.getString("expedited_shipping");Log.i("hello",expedited_shipping);
        String one_day_shipping = bundle.getString("one_day_shipping");Log.i("hello",one_day_shipping);
        String return_accepted = bundle.getString("return_accepted");Log.i("hello",return_accepted);

        TextView shipping_type_s = (TextView)findViewById(R.id.shipping_type_s);
        shipping_type_s.setText(shipping_type);
        TextView handling_time_s = (TextView)findViewById(R.id.handling_time_s);
        handling_time_s.setText(handling_time+" day(s)");
        TextView shipping_loaction_s = (TextView)findViewById(R.id.shipping_location_s);

        shipping_loaction_s.setText(shipping_loaction);
        ImageView expedited_shipping_s = (ImageView)findViewById(R.id.expedited_shipping_s);
        if(expedited_shipping.equals("true") ){
            expedited_shipping_s.setImageResource(R.drawable.gou);
        }
        else{
            expedited_shipping_s.setImageResource(R.drawable.chacha);
        }
        ImageView one_day_shipping_s = (ImageView)findViewById(R.id.one_day_shipping_s);
        if(one_day_shipping.equals("true") ){
            one_day_shipping_s.setImageResource(R.drawable.gou);
        }
        else{
            one_day_shipping_s.setImageResource(R.drawable.chacha);
        }
        ImageView return_accepted_s = (ImageView)findViewById(R.id.return_accepted_s);
        if(return_accepted.equals("true") ){
            return_accepted_s.setImageResource(R.drawable.gou);
        }
        else{
            return_accepted_s.setImageResource(R.drawable.chacha);
        }


        ImageView image_view = (ImageView)findViewById(R.id.image0);
        DownloadImageTask download_img = new DownloadImageTask(image_view);
        download_img.execute(image_url);

        TextView show_title = (TextView)findViewById(R.id.title);
        TextView show_price = (TextView)findViewById(R.id.price);
        TextView show_item_location = (TextView)findViewById(R.id.item_location);
        ImageView top_rated_img = (ImageView)findViewById(R.id.top_rated);
        ImageView facebook_icon = (ImageView)findViewById(R.id.facebook_icon);

        Button show_buy_now = (Button)findViewById(R.id.buy_now);
        show_title.setText(title);
        show_price.setText( "Price:$"+price+"("+ship_cost+" Shipping)" );
        show_item_location.setText(item_location);
        facebook_icon.setImageResource(R.drawable.fb);
        facebook_icon.setOnClickListener(new View.OnClickListener() {
        /*
            @Override
            public void onClick(View view) {
                Profile profile = Profile.getCurrentProfile();
                AccessToken accessToken = AccessToken.getCurrentAccessToken();
                if (profile != null && accessToken != null) {
                    if (accessToken.getPermissions().contains("publish_actions")) {
                        ShareLinkContent shareContent = new ShareLinkContent.Builder()
                                .setContentTitle(title).setContentDescription("Price:$"+price+"("+ship_cost+" shipping),"+"Location: "+item_location)
                                .setContentUrl(Uri.parse(item_url))
                                .build();

                        ShareDialog shareDialog = new ShareDialog(View_detail.this);
                        shareDialog.registerCallback(callbackManager, new FacebookCallback<Sharer.Result>() {
                            @Override
                            public void onSuccess(Sharer.Result result) {
                                Toast.makeText(View_detail.this, "Share Success", Toast.LENGTH_SHORT).show();
                            }

                            @Override
                            public void onCancel() {
                                Toast.makeText(View_detail.this, "Share Cancelled", Toast.LENGTH_SHORT).show();
                            }

                            @Override
                            public void onError(FacebookException exception) {
                                Toast.makeText(View_detail.this, exception.getMessage(), Toast.LENGTH_LONG).show();
                                //Log.e(TAG, "Share: " + exception.getMessage());
                                exception.printStackTrace();
                            }
                        });

                        if (ShareDialog.canShow(ShareLinkContent.class)) {
                            shareDialog.show(shareContent);
                        }
                    } else {
                        List<String> permissions = Arrays.asList("publish_actions");
                        //LoginManager.getInstance().logInWithPublishPermissions(this, permissions);
                    }
                }
            }
        });
        */
            @Override
            public void onClick(View v) {
                if (ShareDialog.canShow(ShareLinkContent.class)) {
                    ShareLinkContent linkContent = new ShareLinkContent.Builder()
                            .setContentTitle(title)
                            .setContentDescription(
                                   "Price:$"+price+"("+ship_cost+" shipping),"+"Location: "+item_location )
                            .setContentUrl(Uri.parse(item_url))
                            .setImageUrl( Uri.parse(image_url) )
                            .build();
                    shareDialog.show(linkContent);

                    //ShareDialog shareDialog = new ShareDialog(View_detail.this);
                    /*
                    shareDialog.registerCallback(callbackManager, new FacebookCallback<Sharer.Result>() {
                        @Override
                        public void onSuccess(Sharer.Result result) {
                            Log.i("facebook","share success");
                            Toast.makeText(View_detail.this, "Share Success", Toast.LENGTH_SHORT).show();
                        }

                        @Override
                        public void onCancel() {
                            Log.i("facebook","cancled");
                            Toast.makeText(View_detail.this, "Share Cancelled", Toast.LENGTH_SHORT).show();
                        }

                        @Override
                        public void onError(FacebookException exception) {
                            Toast.makeText(View_detail.this, exception.getMessage(), Toast.LENGTH_LONG).show();
                            //Log.e(TAG, "Share: " + exception.getMessage());
                            exception.printStackTrace();
                        }
                    });*/
                }
            }
        });

        if( top_rated_item.equals("true") ) {
            top_rated_img.setImageResource(R.drawable.toprated);
        }
        show_buy_now.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(item_url));
                startActivity(browserIntent);
            }
        });
        // show detail of basic info button
        Button basic_info =(Button)findViewById(R.id.basic_info);
        Button seller =(Button)findViewById(R.id.seller);
        Button shipping =(Button)findViewById(R.id.shipping);
        basic_info.setBackgroundResource(android.R.drawable.btn_default);
        seller.setBackgroundResource(android.R.drawable.btn_default);
        shipping.setBackgroundResource(android.R.drawable.btn_default);
        basic_info.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                RelativeLayout rl1 = (RelativeLayout) findViewById(R.id.seller_layout);
                rl1.setVisibility(View.INVISIBLE);
                RelativeLayout rl3 = (RelativeLayout) findViewById(R.id.shipping_layout);
                rl3.setVisibility(View.INVISIBLE);
                RelativeLayout rl2 = (RelativeLayout) findViewById(R.id.basic_info_layout);
                rl2.setVisibility(View.VISIBLE);
                Button basic_info =(Button)findViewById(R.id.basic_info);
                Button seller =(Button)findViewById(R.id.seller);
                Button shipping =(Button)findViewById(R.id.shipping);
                basic_info.setBackgroundColor(Color.parseColor("#B1B7C3"));
                seller.setBackgroundResource(android.R.drawable.btn_default);
                shipping.setBackgroundResource(android.R.drawable.btn_default);
            }
        });
        //show detail of seller button

        seller.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                RelativeLayout r21 = (RelativeLayout) findViewById(R.id.seller_layout);
                r21.setVisibility(View.VISIBLE);
                RelativeLayout r23 = (RelativeLayout) findViewById(R.id.shipping_layout);
                r23.setVisibility(View.INVISIBLE);
                RelativeLayout r22 = (RelativeLayout) findViewById(R.id.basic_info_layout);
                r22.setVisibility(View.INVISIBLE);
                Button basic_info =(Button)findViewById(R.id.basic_info);
                Button seller =(Button)findViewById(R.id.seller);
                Button shipping =(Button)findViewById(R.id.shipping);
                basic_info.setBackgroundResource(android.R.drawable.btn_default);
                seller.setBackgroundColor(Color.parseColor("#B1B7C3"));
                shipping.setBackgroundResource(android.R.drawable.btn_default);
            }
        });
        //show detail of shipping button

        shipping.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                RelativeLayout r31 = (RelativeLayout) findViewById(R.id.seller_layout);
                r31.setVisibility(View.INVISIBLE);
                RelativeLayout r33 = (RelativeLayout) findViewById(R.id.shipping_layout);
                r33.setVisibility(View.VISIBLE);
                RelativeLayout r32 = (RelativeLayout) findViewById(R.id.basic_info_layout);
                r32.setVisibility(View.INVISIBLE);
                Button basic_info =(Button)findViewById(R.id.basic_info);
                Button seller =(Button)findViewById(R.id.seller);
                Button shipping =(Button)findViewById(R.id.shipping);
                seller.setBackgroundResource(android.R.drawable.btn_default);
                basic_info.setBackgroundResource(android.R.drawable.btn_default);
                shipping.setBackgroundColor(Color.parseColor("#B1B7C3"));
            }
        });
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_view_detail, menu);
        return true;
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
