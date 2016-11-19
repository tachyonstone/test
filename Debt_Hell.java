import java.io.*;

class Cal
{
	protected int i;
	protected double debt = 100000;
	protected double interest;


	public double ans(int week){
		for (i=0; i<week; i++) {
			interest = debt*0.05;

			if (interest%1000 != 0) {
				interest += 1000;
			}

			debt = debt + interest;
			debt = debt/1000;
			debt = debt*1000;

		}

		return debt;
	}
}


class Debt_Hell
{
	public static void main(String[] args) throws IOException
	 {
		BufferedReader br =
			new BufferedReader(new InputStreamReader (System.in));

		String str = br.readLine();

		int week = Integer.parseInt(str);

		Cal cal1 = new Cal();

		double ans = cal1.ans(week);

		System.out.println(ans);



	}




}