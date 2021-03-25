using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ArtcCliLibrary;


namespace ArCliDemo
{
    public partial class Form1 : Form
    {
        private ArtcCli artc = new ArtcCli();
        private ArtcCliLibrary.onCameraReady cameraReadyEvent;
        private System.Timers.Timer m_timer;
        private bool m_start;
        private bool m_pulling;

        public Form1()
        {
            InitializeComponent();
            log("initialize", artc.initialize(txtVendorkey.Text));
            //log("initialize rtm", agoraRTM.initialize(txtVendorkey.Text));
            artc.onCameraReady += onCameraReady;
            artc.onJoinChannelSuccess += onJoinChannelSuccess;
            artc.onFirstRemoteVideoDecoded += onFirstRemoteVideoDecoded;
            artc.onFirstLocalVideoFrame += onFirstLocalVideoFrame;
            artc.onCaptureVideoFrame += onCaptureVideoFrame;
            artc.onPreEncodeVideoFrame += onPreEncodeVideoFrame;

            /*AgoraCliRTMChannel channel = agoraRTM.createChannel("1234");
            channel.onMemberJoined += onMemberJoined;*/

        }

        bool onPreEncodeVideoFrame(CliVideoFrame frame)
        {
            //log("onPreEncodeVideoFrame", (int)frame.type);
            return true;
        }

        private void onFirstLocalVideoFrame(int width, int height, int elapsed)
        {
            log("on first local video frame with " + width + "," + height + "," + elapsed, 0);
        }

        private bool onCaptureVideoFrame(ArtcCliLibrary.CliVideoFrame frame)
        {
            return true;
        }

        private void onFirstRemoteVideoDecoded(String uid, int width, int height, int elapsed)
        {
            log("remote video decoded in " + uid, elapsed);
            setupRemoteVideo(uid);
        }

        delegate void setupRemoteVideoDelegate(String uid);
        private void setupRemoteVideo(String uid)
        {
            if (remoteVideo.InvokeRequired)
            {
                setupRemoteVideoDelegate d = new setupRemoteVideoDelegate(setupRemoteVideo);
                this.Invoke(d, uid);
            } else
            {
                artc.setupRemoteVideo(remoteVideo.Handle, (int)EnumRenderModeType.RENDER_MODE_ADAPTIVE, uid);
            }
        }

        private void onCameraReady()
        {
            log("Camera Ready Event", 0);
        }

        private void onJoinChannelSuccess(String channel, String uid, int elapsed)
        {
            log("join channel success in " + channel + " by " + uid, elapsed);
            m_start = true;
        }

        delegate void LogDelegate(String operation, int result);

        private void log(String operation, int result)
        {
            if (txtResult.InvokeRequired)
            {
                LogDelegate d = new LogDelegate(log);
                this.Invoke(d, new object[] { operation, result });
            } else
            {
                txtResult.AppendText(String.Format("artc {0} result is {1}", operation, result));
                txtResult.AppendText(Environment.NewLine);
            }

        }

        private void button1_Click(object sender, EventArgs e)
        {
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void txtResult_TextChanged(object sender, EventArgs e)
        {

        }

        private void btnStartPreview_Click(object sender, EventArgs e)
        {
            log("setChannelProfile", artc.setChannelProfile(ChannelProfile.CHANNEL_PROFILE_LIVE_BROADCASTING));
            log("setClientRole", artc.setClientRole(ClientRoleType.CLIENT_ROLE_BROADCASTER));
            log("enableVideo", artc.enableVideo());
            log("setuplocalVideo", artc.setupLocalVideo(localVideo.Handle, (int)EnumRenderModeType.RENDER_MODE_ADAPTIVE, "0"));
            log("setExternalAudioSink", artc.setExternalAudioSink(true, 44100, 1));
            log("startPreview", artc.startPreview());

            m_timer = new System.Timers.Timer(10);
            m_timer.Elapsed += pullAudioFrameTimer;
            m_timer.Enabled = true;
        }

        private void pullAudioFrameTimer(object sender, System.Timers.ElapsedEventArgs e)
        {
            if (!m_start) return;
            if (m_pulling) return;

            m_pulling = true;
            CliAudioFrame frame = null;
            var result = artc.pullAudioFrame(out frame);
            m_pulling = false;
        }

        private void btnJoinChannel_Click(object sender, EventArgs e)
        {
            log("join channel", artc.joinChannel(txtToken.Text, txtChannelName.Text, null, null));
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            //log(" convert -1 to unit result is " + Convert.ToUInt32(-1), 0);
            //log("start audio recording", artc.startAudioRecording("recording.mp4", AudioRecordingQualityType.AUDIO_RECORDING_QUALITY_HIGH));
            artc.release();
        }
    }
}
