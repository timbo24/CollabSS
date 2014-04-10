namespace SS
{
    partial class FormLogin
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.NameTextBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.ConnectButton = new System.Windows.Forms.Button();
            this.BigTextBox = new System.Windows.Forms.TextBox();
            this.EnterWordTextBox = new System.Windows.Forms.TextBox();
            this.EnterWordButton = new System.Windows.Forms.Button();
            this.IPAddrBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.DisconnectButton = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // NameTextBox
            // 
            this.NameTextBox.Location = new System.Drawing.Point(201, 116);
            this.NameTextBox.Name = "NameTextBox";
            this.NameTextBox.Size = new System.Drawing.Size(122, 20);
            this.NameTextBox.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Times New Roman", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(103, 118);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(90, 15);
            this.label1.TabIndex = 1;
            this.label1.Text = "Enter password";
            // 
            // ConnectButton
            // 
            this.ConnectButton.Location = new System.Drawing.Point(201, 196);
            this.ConnectButton.Name = "ConnectButton";
            this.ConnectButton.Size = new System.Drawing.Size(75, 23);
            this.ConnectButton.TabIndex = 2;
            this.ConnectButton.Text = "Connect";
            this.ConnectButton.UseVisualStyleBackColor = true;
            this.ConnectButton.Click += new System.EventHandler(this.ConnectButton_Click);
            // 
            // BigTextBox
            // 
            this.BigTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.BigTextBox.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
            this.BigTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.BigTextBox.Font = new System.Drawing.Font("Times New Roman", 12F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.BigTextBox.Location = new System.Drawing.Point(118, 67);
            this.BigTextBox.Multiline = true;
            this.BigTextBox.Name = "BigTextBox";
            this.BigTextBox.ReadOnly = true;
            this.BigTextBox.Size = new System.Drawing.Size(327, 32);
            this.BigTextBox.TabIndex = 3;
            this.BigTextBox.Visible = false;
            // 
            // EnterWordTextBox
            // 
            this.EnterWordTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.EnterWordTextBox.Location = new System.Drawing.Point(201, 257);
            this.EnterWordTextBox.Name = "EnterWordTextBox";
            this.EnterWordTextBox.Size = new System.Drawing.Size(122, 20);
            this.EnterWordTextBox.TabIndex = 4;
            this.EnterWordTextBox.Visible = false;
            // 
            // EnterWordButton
            // 
            this.EnterWordButton.Location = new System.Drawing.Point(201, 292);
            this.EnterWordButton.Name = "EnterWordButton";
            this.EnterWordButton.Size = new System.Drawing.Size(73, 23);
            this.EnterWordButton.TabIndex = 5;
            this.EnterWordButton.Text = "Enter Name";
            this.EnterWordButton.UseVisualStyleBackColor = true;
            this.EnterWordButton.Visible = false;
            this.EnterWordButton.Click += new System.EventHandler(this.EnterWordButton_Click);
            // 
            // IPAddrBox
            // 
            this.IPAddrBox.Location = new System.Drawing.Point(201, 159);
            this.IPAddrBox.Name = "IPAddrBox";
            this.IPAddrBox.Size = new System.Drawing.Size(122, 20);
            this.IPAddrBox.TabIndex = 9;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Times New Roman", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(129, 161);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(66, 15);
            this.label3.TabIndex = 10;
            this.label3.Text = "IP Address";
            // 
            // label4
            // 
            this.label4.Font = new System.Drawing.Font("Comic Sans MS", 18F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label4.Location = new System.Drawing.Point(160, 12);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(392, 42);
            this.label4.TabIndex = 12;
            this.label4.Text = "Collaborative Spreadsheet Login";
            // 
            // DisconnectButton
            // 
            this.DisconnectButton.Location = new System.Drawing.Point(634, 12);
            this.DisconnectButton.Name = "DisconnectButton";
            this.DisconnectButton.Size = new System.Drawing.Size(75, 23);
            this.DisconnectButton.TabIndex = 26;
            this.DisconnectButton.Text = "Disconnect";
            this.DisconnectButton.UseVisualStyleBackColor = true;
            this.DisconnectButton.Click += new System.EventHandler(this.DisconnectButton_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Times New Roman", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(48, 259);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(147, 15);
            this.label2.TabIndex = 27;
            this.label2.Text = "Name of your spreadsheet";
            this.label2.Visible = false;
            // 
            // FormLogin
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
            this.ClientSize = new System.Drawing.Size(721, 420);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.DisconnectButton);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.IPAddrBox);
            this.Controls.Add(this.BigTextBox);
            this.Controls.Add(this.ConnectButton);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.NameTextBox);
            this.Controls.Add(this.EnterWordTextBox);
            this.Controls.Add(this.EnterWordButton);
            this.Name = "FormLogin";
            this.Text = "Work on your spreadsheet.";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox NameTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button ConnectButton;
        private System.Windows.Forms.TextBox BigTextBox;
        private System.Windows.Forms.TextBox EnterWordTextBox;
        private System.Windows.Forms.Button EnterWordButton;
        private System.Windows.Forms.TextBox IPAddrBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button DisconnectButton;
        private System.Windows.Forms.Label label2;
    }
}

