package com.example.john.myfirstapp;
import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import android.content.Intent;
import android.os.AsyncTask;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Spinner;
import android.widget.TextView;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.StatusLine;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;
import org.json.JSONException;
import org.json.JSONObject;

/*
public class DownloadFilesTask extends AsyncTask<String, Integer,String> {
    protected Long doInBackground(URL) {
        int count = urls.length;
        long totalSize = 0;
        for (int i = 0; i < count; i++) {
            totalSize += Downloader.downloadFile(urls[i]);
            publishProgress((int) ((i / (float) count) * 100));
            // Escape early if cancel() is called
            if (isCancelled()) break;
        }
        return totalSize;
    }

    protected void onProgressUpdate(Integer... progress) {
        setProgressPercent(progress[0]);
    }

    protected void onPostExecute(Long result) {
        showDialog("Downloaded " + result + " bytes");
    }
}
        */

public class MyActivity extends ActionBarActivity {
    public final static String EXTRA_MESSAGE = "com.mycompany.myfirstapp.MESSAGE";

    public class send_asyncTask extends AsyncTask<String, Void,String> {
        @Override
        protected String doInBackground(String... urls) {
            Log.i("debug","I MA HERE");
            String response ="";
            for (String url : urls){
                DefaultHttpClient client = new DefaultHttpClient();
                HttpGet httpGet = new HttpGet(url);
                try {
                    HttpResponse execute = client.execute(httpGet);
                    InputStream content = execute.getEntity().getContent();
                    BufferedReader buffer = new BufferedReader(new InputStreamReader(content));
                    String s = "";
                    while ((s = buffer.readLine()) != null) {
                        response += s;
                    }
                }
                catch (Exception e) {
                    Log.i("debug","exception error");
                    e.printStackTrace();
                }
            }
            return response;
        }
        @Override
        protected void onPostExecute(String result) {
            Log.i("debug",""+result);
            Intent intent = new Intent(MyActivity.this,DisplayMessageActivity.class);
            Bundle bundle1 =new Bundle();
            EditText keyword = ( EditText )findViewById(R.id.keyword_blank);
            String keyword_str = keyword.getText().toString();
            bundle1.putString("keyword",keyword_str);
            //bundle1.putString("keyword",keyword_str);
            Log.i("debug",""+keyword);

            bundle1.putString("json",result);
            intent.putExtras(bundle1);
            if(result.equals( "{\"ack\":\"fail\"}" )) {
                Log.i("debug","failure goes here");
                TextView no_result =(TextView)findViewById(R.id.show_validation);
                no_result.setText("No Results Found");
            }else {
                startActivity(intent);
            }
            Log.i("debug","after startActivity");
        }
    }

    public static boolean isNumeric(String str){
        try
        {
            double d = Double.parseDouble(str);
        }
        catch(NumberFormatException nfe)
        {
            return false;
        }
        return true;
    }
    public void clear_f(View view){
        EditText keyword = ( EditText )findViewById(R.id.keyword_blank);
        EditText price_from = (EditText)findViewById(R.id.price_from_blank);
        EditText price_to = (EditText)findViewById(R.id.price_to_blank);
        Spinner sort_by = (Spinner)findViewById(R.id.sort_by_blank);

        keyword.setText("");
        price_from.setText("");
        price_to.setText("");
        sort_by.setSelection(0);
        //findViewById(R.id.sort_by_blank);
    }
    public void validate_f(View view){
        EditText keyword = ( EditText )findViewById(R.id.keyword_blank);
        EditText price_from = (EditText)findViewById(R.id.price_from_blank);
        EditText price_to = (EditText)findViewById(R.id.price_to_blank);
        Spinner sort_by = (Spinner)findViewById(R.id.sort_by_blank);
        TextView show_validation =(TextView)findViewById(R.id.show_validation);
        String keyword_str = keyword.getText().toString();
        String from_str  = price_from.getText().toString();
        String to_str = price_to.getText().toString();
        String sortby_str = sort_by.getSelectedItem().toString();

        int validate_flag =1;
        //int from_int = Integer.parseInt( from_str );
        //int to_int = Integer.parseInt( to_str );
        Log.i("debug","debug works");
        if( !from_str.equals("") ){
            if(isNumeric(from_str)){
                show_validation.setText("");
                float from_int = Float.parseFloat( from_str );
                if(from_int >=0){
                    show_validation.setText("");
                    if(isNumeric(to_str)){
                        show_validation.setText("");
                        float to_int = Float.parseFloat( to_str );
                        if( from_int<=to_int){
                            show_validation.setText("");
                        }
                        else{
                            show_validation.setText("price to should be greater than price from");
                            validate_flag = 0;
                        }
                    }
                }
                else{
                    show_validation.setText("price from should be greater than or equal to 0");
                    validate_flag = 0;
                }
            }
            else{
                show_validation.setText("Price from should be a Number");
                validate_flag =0;
            }
        }
        if(!to_str.equals("") && validate_flag == 1){
            if(isNumeric(to_str)){
                show_validation.setText("");
                float to_int = Float.parseFloat( to_str );
                if(to_int >=0){
                    show_validation.setText("");
                }
                else{
                    show_validation.setText("price to should be greater than or equal to 0");
                }
            }
            else{
                show_validation.setText("Price to should be a Number");
                validate_flag =0;
            }
        }
        if( keyword.getText().toString().matches("") ){
            show_validation.setText("Please enter a keyword");
            validate_flag = 0;
        }
        else{
            if(validate_flag == 1)
                show_validation.setText("");
        }
        if( validate_flag == 1){
            //Log.i("debug","1");
            send_asyncTask mytask = new send_asyncTask();
            //Log.i("debug","2");
            String url ="http://indexproj-env.elasticbeanstalk.com/index/hw8.php?";
            Log.i("debug",keyword_str);
            //Log.i("debug",from_str);
            //Log.i("debug",to_str);
            if( sortby_str.equals("Best Match") ){
                sortby_str = "BestMatch";
            }
            else if( sortby_str.equals("Price:highest first") ){
                sortby_str = "CurrentPriceHighest";
            }
            else if( sortby_str.equals("Price+Shipping:highest first") ){
                sortby_str = "PricePlusShippingHighest";
            }
            else{
                sortby_str = "PricePlusShippingLowest";
            }
            Log.i("debug",sortby_str);
            url = url+ "Keywords="+keyword_str +"&MinPrice="+from_str+"&MaxPrice="+to_str+"&sortOrder="+sortby_str+"&paginationInput=5&pageNumber=1";
            mytask.execute(url);
            //mytask.execute("http://indexproj-env.elasticbeanstalk.com/index/hw8.php?Keywords=shoes&MinPrice=11&MaxPrice=22&MaxHandlingTime=&sortOrder=BestMatch&paginationInput=5&pageNumber=1");
            //mytask.execute("http://indexproj-env.elasticbeanstalk.com/index/hw8.php?Keywords=baseball&MinPrice=11&MaxPrice=22222&Condition%5B%5D=1000&Condition%5B%5D=3000&ReturnsAcceptedOnly%5B%5D=true&FreeShippingOnly%5B%5D=true&MaxHandlingTime=1&sortOrder=PricePlusShippingHighest&paginationInput=10&pageNumber=1");
            Log.i("debug","3");
        }
    }
/*
    public void sendMessage(View view) {
        Intent intent = new Intent(this,DisplayMessageActivity.class);
        EditText editText = (EditText) findViewById(R.id.edit_message);
        String message = editText.getText().toString();
        intent.putExtra(EXTRA_MESSAGE, message);
        startActivity(intent);
    }
*/
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //FacebookSdk.sdkInitialize(getApplicationContext());
        setContentView(R.layout.activity_my);
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_my, menu);
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
