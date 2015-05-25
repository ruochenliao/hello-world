package algorithm;
import java.util.Arrays;
import java.util.Scanner;

public class myFirstApp {
	public static void main(String[] args){
		Scanner user_input = new Scanner(System.in);
		String str1 = user_input.next();
		String str2 = user_input.next();
		System.out.println( two_String_permutation(str1,str2) );
	}
	public static boolean two_String_permutation(String a, String b){
		if(a.length()!=b.length())
			return false;
		return String_sort(a).equals( String_sort(b) );
	}
	
	public static String String_sort(String str){
		char[] _char = str.toCharArray();
		Arrays.sort(_char);
		String a = new String(_char);
		return a;
	}
		
}
