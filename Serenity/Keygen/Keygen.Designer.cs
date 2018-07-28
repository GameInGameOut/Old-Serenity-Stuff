namespace Keygen
{
    partial class Keygen
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
            this.butRegister = new System.Windows.Forms.Button();
            this.txtKey = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // butRegister
            // 
            this.butRegister.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.butRegister.Font = new System.Drawing.Font("Lucida Sans Unicode", 12F);
            this.butRegister.Location = new System.Drawing.Point(12, 60);
            this.butRegister.Name = "butRegister";
            this.butRegister.Size = new System.Drawing.Size(254, 32);
            this.butRegister.TabIndex = 6;
            this.butRegister.Text = "Register User";
            this.butRegister.UseVisualStyleBackColor = true;
            this.butRegister.Click += new System.EventHandler(this.butRegister_Click);
            // 
            // txtKey
            // 
            this.txtKey.Font = new System.Drawing.Font("Lucida Sans Unicode", 12F);
            this.txtKey.ForeColor = System.Drawing.Color.Black;
            this.txtKey.Location = new System.Drawing.Point(12, 12);
            this.txtKey.Name = "txtKey";
            this.txtKey.Size = new System.Drawing.Size(254, 32);
            this.txtKey.TabIndex = 5;
            this.txtKey.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // Keygen
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(282, 110);
            this.Controls.Add(this.butRegister);
            this.Controls.Add(this.txtKey);
            this.Name = "Keygen";
            this.Text = "Keygen";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button butRegister;
        private System.Windows.Forms.TextBox txtKey;
    }
}

