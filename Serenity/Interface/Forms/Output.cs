using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Interface.Forms
{
    public partial class Output : Form
    {
        public static string strOutput;

        public Output()
        {
            InitializeComponent();
        }

        private void Output_Load(object sender, EventArgs e)
        {
            txtPassword.Text = strOutput;
        }

        private void butExit_Click(object sender, EventArgs e)
        {
            Properties.Settings.Default.FirstLaunch = false;
            Properties.Settings.Default.Save();
            Application.Exit();
        }

        private void Output_FormClosing(object sender, FormClosingEventArgs e)
        {
            Properties.Settings.Default.FirstLaunch = false;
            Properties.Settings.Default.Save();
        }

        private void Output_FormClosed(object sender, FormClosedEventArgs e)
        {
            Properties.Settings.Default.FirstLaunch = false;
            Properties.Settings.Default.Save();
        }
    }
}
