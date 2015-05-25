package algorithm;
import java.util.ArrayList;

public class simpleDocCom {
	//int numOfHits = 0;
	//int[] locations = new int[3];
	private ArrayList<String> locations = new ArrayList();
	public void setLocation(ArrayList<String> locs){
		locations = locs;
	}
	public String checkYourself(String userGuess){
		System.out.print("guess "+ userGuess+" ");
		//int index = locations.indexOf(userGuess);
		if( locations.contains(userGuess) ){
			locations.remove(userGuess);
			if( locations.isEmpty() ){
				return "killed";
			}
			else{
				return "hit";
			}
			/*
			if(index==0){
				return "killed";
			}
			else{
				return "hit";
			}
			*/
		}
		else{
			return "fails";
		}


		
	}
}
