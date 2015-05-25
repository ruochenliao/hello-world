package algorithm;
import java.util.Arrays;
import java.util.Scanner;

public class myFirstApp {
	public static void main(String[] args){
		Scanner user_input = new Scanner(System.in);
		String str1 = user_input.nextLine();
		int length_int = user_input.nextInt();
		char[] a = str1.toCharArray();
		myFirstApp.replace_space(a,length_int);
		System.out.println(a);
	}
	public static void replace_space(char[] str_char, int true_length){
		int i;
		int new_length = str_char.length;
		for(i = true_length - 1; i >=0 ; i--){
			if( str_char[i]==' '){
				str_char[new_length-1] = '0';
				str_char[new_length-2] = '2';
				str_char[new_length-3] = '%';
				new_length = new_length -3;
			}
			else{
				str_char[new_length-1] =str_char[i];
				new_length =new_length - 1;
			}
		}
	}
	
}
