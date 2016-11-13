clear all
load digit.mat X T    % X は訓練用データ（500文字/数字），Tは評価用データ
[d, n, nc] = size(X); % d=256, n=500, nc=10

Z = reshape(X,[d n*nc]); % Z は 256x5000 行列．全データを並べたもの．

% 分散・共分散行列 V の計算
V = cov(Z');  % これで一挙に V を計算！

% 正定値対称行列 V の固有ベクトル・固有値の計算
[eigvec eigval] = eig(V);
eigval = eig(V);

% ここで固有ベクトルを固有値の大きい順に並べ替える必要あり．
[s, index] = sort(eigval, 'descend');
eigvec = eigvec(:,index); % eigvec は 256x256 行列
e = eigvec(:,[1,2]); % e は 256x2 行列


x1 = X(:,23,1); % x は数字「1」の23番目の例．256次元の縦ベクトル．
x2 = X(:,23,3); % x は数字「3」の23番目の例．256次元の縦ベクトル．
x3 = X(:,23,8); % x は数字「8」の23番目の例．256次元の縦ベクトル．


s = zeros(256,1);


%数字「1」について
for m=1:256
for c=1:m
   a = x1'*eigvec(:,c); % 第c主成分の重みを内積で求める．
   s = s + a*eigvec(:,c); % 主成分の重み付け和で画像xを再構成
end



		r = norm(x1-s);
		A(m,1) = m;
		A(m,2) = r;


s = 0;


end


%数字「3」について
for m=1:256
for c=1:m
   a = x2'*eigvec(:,c); % 第c主成分の重みを内積で求める．
   s = s + a*eigvec(:,c); % 主成分の重み付け和で画像xを再構成
end



r = norm(x2-s);
		B(m,1) = m;
		B(m,2) = r;


s = 0;


end


%数字「8」について
for m=1:256
for c=1:m
   a = x3'*eigvec(:,c); % 第c主成分の重みを内積で求める．
   s = s + a*eigvec(:,c); % 主成分の重み付け和で画像xを再構成
end



		r = norm(x3-s);
		C(m,1) = m;
		C(m,2) = r;


s = 0;


end


%A

%B

%C

		figure(21);
		clf;
		plot(A(:,1),A(:,2),'r-');

		print -depsc2 error_graph_1.eps

		figure(22);
		clf;
	 	plot(B(:,1),B(:,2),'g-');

		print -depsc2 error_graph_3.eps

		figure(23);
		clf;
		plot(C(:,1),C(:,2),'b-');

		print -depsc2 error_graph_8.eps

		figure(30);
		clf;
		plot(A(:,1),A(:,2),'r-');
		hold on;
	 	plot(B(:,1),B(:,2),'g-');
		hold on;
		plot(C(:,1),C(:,2),'b-');

		print -depsc2 error_graph.eps
