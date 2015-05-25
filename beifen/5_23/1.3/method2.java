package algorithm;
import java.util.Arrays;
import java.util.Scanner;

public class myFirstApp {
	public static void main(String[] args){
		Scanner user_input = new Scanner(System.in);
		String str1 = user_input.nextLine();
		String str2 = user_input.nextLine();
		boolean isSubString = two_String_permutation(str1,str2);
		System.out.println(isSubString);
	}
	
	
	public static boolean two_String_permutation(String str1, String str2){
		char[] char1= str1.toCharArray();
		char[] char2= str2.toCharArray();
		int[] store = new int[256];
		for(char c: char1)
			store[c] ++;
		for(int i =0; i< char2.length;i++){
			if( --store[ char2[i] ]<0 )
				return false;
		}
		return true;
	}
	
}
