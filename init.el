;;from ext
;;C-\\でブロックを折り畳む(C言語のみ)
(add-hook 'c-mode-hook
'(lambda()
(hs-minor-mode 1)))
(define-key global-map(kbd"C-\\") 'hs-toggle-hidding)
;;タブ幅4
(setq default-tab-width 4)
;;cuamode(矩形編集)
;;範囲選択中に，C-<enter>で矩形選択モードになる．
(cua-mode t)
;;そのままだとC-xが切り取りになってしまったりするので無効化
(setq cua-enable-cua-keys nil)
;; 言語設定
(set-language-environment 'Japanese)
;; UTF-8を使用
(prefer-coding-system 'utf-8)
(set-default-coding-systems 'utf-8)
;; 対応括弧ハイライト
(show-paren-mode t)
;; ファイル名補完で大文字小文字を区別しない
(setq completion-ignore-case t)
;; カーソル位置の桁数をモードライン行に表示する
(column-number-mode 1)
;; カーソル位置の行数をモードライン行に表示する
(line-number-mode 1)
;; オープニングメッセージ消去
(setq inhibit-startup-message -1)
;; メニューバーを消す
;;(menu-bar-mode -1)
;; ツールバーを隠す
;;(tool-bar-mode -1)
;; 行番号表示
(global-linum-mode t)
;; *.~ とかのバックアップファイルを作らない
;;(setq make-backup-files nil)
;; .#* とかのバックアップファイルを作らない
;;(setq auto-save-default nil)
;;テーマの読み込み
;(load-theme 'adwaita t)
;; 現在行ハイライト
(global-hl-line-mode t)
;; 行末空白のハイライト
(setq-default show-trailing-whitespace t)
(set-face-background 'trailing-whitespace "cyan")
;; 保存前に行末の空白削除
(add-hook 'before-save-hook 'delete-trailing-whitespace)
(defun all-indent ()
(interactive)
(let((pos(point)))
(mark-whole-buffer)
(indent-region (region-beginning)(region-end))
(goto-char pos)))
(global-set-key (kbd "C-x C-]") 'all-indent)
(put 'upcase-region 'disabled nil)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Color
;;(if window-system (progn
   ; (set-background-color "Black")
   ; (set-foreground-color "LightGray")
   ; (set-cursor-color "Gray")
   ; (set-frame-parameter nil 'alpha 90) ;透明度
 ;; ))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; auto-completeの導入
; 補完入力機能
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 下記の3行を追記。このとき、そのさらに2行下の記述はコメントアウト
;; emacsを起動して M-x package-install [RET]
;; auto-complete [RET]
(require 'package)
(add-to-list 'package-archives '("melpa" . "http://melpa.milkbox.net/packages/"))
(package-initialize)

;; 下の2行は上の操作をした後に追記
(require 'auto-complete-config)
(ac-config-default)


;;;;;;;;;;;;;;;;;;;;;;;
; 閉じかっこの自動補完
(electric-pair-mode 1)
