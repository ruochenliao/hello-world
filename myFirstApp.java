package algorithm;
import java.util.ArrayList;
public class myFirstApp {
	String name= new String("this is my name");
	String[] id = new String[2];	
	public static void main(String[] arg){		
		simpleDocCom doc = new simpleDocCom();
		int randomNum = (int)(Math.random()*5);
		System.out.println(randomNum+" "+(randomNum+1)+" "+ (randomNum+2) );	
		ArrayList<String> location = new ArrayList<String>();
		location.add( Integer.toString(randomNum) );
		location.add( Integer.toString(randomNum+1) );
		location.add( Integer.toString(randomNum+2) );
		doc.setLocation (location);
		int  i = 0;
		while(true){
			i++;
			String userGuess = Integer.toString((int)(Math.random()*10) );
			String result = doc.checkYourself(userGuess);
			if( result.equals("hit") ){
				System.out.println("hit");
			}
			if( result.equals("fails")){
				System.out.println("fails");
			}
			if(result.equals("killed")){
				System.out.println("killed by guessing" + i +" times");
				break;		
			}
		}
	}
}

