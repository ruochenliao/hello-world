package algorithm;
import java.util.ArrayList;
public class myFirstApp {
	private gameHelper helper = new gameHelper();
	private ArrayList<simpleDocCom> dotComsList = new ArrayList<simpleDocCom>();
	private int numOfGuesses = 0;
	
	private void setUpGame(){
			simpleDocCom one = new simpleDocCom();
			one.setName("Pets.com");
			simpleDocCom two = new simpleDocCom();
			two.setName("eToys.com");
			simpleDocCom three = new simpleDocCom();
			three.setName("Go2.com");
			dotComsList.add(one);
			dotComsList.add(two) ;
			dotComsList.add(three);
			for(simpleDocCom dotComToSet:dotComsList){
				ArrayList<String> newLocation = helper.placeDotCom(3);
				dotComToSet. setLocationCells (newLocation);
			}
	}
	private void startPlaying(){
		while( !dotComsList.isEmpty() ){
			String userGuess = helper.getUserlnput ("Enter a guess");
			checkUserGuess(userGuess);
		}
	}
	private void checkUserGuess(String userGuess){
		numOfGuesses ++;
		String result ="miss";
		for( simpleDocCom dotComToTest: dotComsList ){
			result = dotComToTest.checkYourself(userGuess);
			if(result.equals("hit")){
				break;
			}
			if(result.equals("kill")){
				dotComsList.remove(dotComToTest);
				break;
			}
		}
		System.out.println(result);
		
	}
	private void finishGame(){
		System.out.println("All not Coms are dead! Your stock is now worthless");
		if( numOfGuesses<18 ){
			System.out.println("It only took you  "+ numOfGuesses + " guesses.") ;
		}
		else{
			System. out.println("Took you long enough. "+ numOfGuesses + " guesses.");
		}
	}
	public static void main(String[] args){
		myFirstApp game = new myFirstApp();
		game.setUpGame();
		game.startPlaying();
	}
	
}
