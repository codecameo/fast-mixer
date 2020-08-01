package com.bluehub.fastmixer.screens.recording

import android.content.Intent
import android.content.IntentFilter
import android.media.AudioManager
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.databinding.DataBindingUtil
import androidx.lifecycle.Observer
import androidx.lifecycle.ViewModelProviders
import androidx.navigation.fragment.findNavController
import com.bluehub.fastmixer.R
import com.bluehub.fastmixer.broadcastReceivers.AudioDeviceChangeListener
import com.bluehub.fastmixer.common.permissions.PermissionFragment
import com.bluehub.fastmixer.common.permissions.PermissionViewModel
import com.bluehub.fastmixer.common.utils.DialogManager
import com.bluehub.fastmixer.common.utils.ScreenConstants
import com.bluehub.fastmixer.databinding.RecordingScreenBinding
import timber.log.Timber
import javax.inject.Inject


class RecordingScreen : PermissionFragment() {

    companion object {
        fun newInstance() = RecordingScreen()
    }

    override var TAG: String = javaClass.simpleName

    private lateinit var dataBinding: RecordingScreenBinding

    @Inject
    override lateinit var dialogManager: DialogManager

    @Inject
    lateinit var audioDeviceChangeListener: AudioDeviceChangeListener

    override lateinit var viewModel: PermissionViewModel
    private lateinit var viewModelFactory: RecordingScreenViewModelFactory

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        getPresentationComponent().inject(this)
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        dataBinding = DataBindingUtil
            .inflate(inflater, R.layout.recording_screen, container, false)

        viewModelFactory = RecordingScreenViewModelFactory(context, TAG)
        viewModel = ViewModelProviders.of(this, viewModelFactory)
            .get(RecordingScreenViewModel::class.java)

        dataBinding.viewModel = viewModel as RecordingScreenViewModel

        dataBinding.lifecycleOwner = viewLifecycleOwner

        setPermissionEvents()
        initUI()

        return dataBinding.root
    }

    override fun onResume() {
        super.onResume()

        val localViewModel = viewModel as RecordingScreenViewModel

        audioDeviceChangeListener.setRestartInputCallback(localViewModel.restartInputStreams)

        audioDeviceChangeListener.setRestartOutputCallback(localViewModel.restartOutputStreams)

        val filter = IntentFilter().apply {
            addAction(AudioManager.ACTION_HEADSET_PLUG)
            addAction(AudioManager.ACTION_SCO_AUDIO_STATE_UPDATED)
        }
        context?.registerReceiver(audioDeviceChangeListener, filter)
    }

    override fun onPause() {
        super.onPause()
        context?.unregisterReceiver(audioDeviceChangeListener)
    }

    fun initUI() {
        val localViewModel = viewModel as RecordingScreenViewModel

        localViewModel.eventIsRecording.observe(viewLifecycleOwner, Observer { isRecording ->
            if (isRecording) {
                dataBinding.toggleRecord.text = getString(R.string.stop_recording_label)
            } else {
                dataBinding.toggleRecord.text = getString(R.string.start_recording_label)
            }
        })

        localViewModel.eventIsPlaying.observe(viewLifecycleOwner, Observer { isPlaying ->
            if (!isPlaying) {
                dataBinding.togglePlay.text = getString(R.string.play_label)
            } else {
                dataBinding.togglePlay.text = getString(R.string.pause_label)
            }
        })

        localViewModel.eventGoBack.observe(viewLifecycleOwner, Observer { goBack ->
            if (goBack) {
                findNavController().navigate(RecordingScreenDirections.actionRecordingScreenToMixingScreen())
                localViewModel.resetGoBack()
            }
        })

        localViewModel.eventRecordPermission.observe(viewLifecycleOwner, Observer { record ->
            if (record.fromCallback && record.hasPermission) {
                when(record.permissionCode) {
                    ScreenConstants.TOGGLE_RECORDING -> localViewModel.toggleRecording()
                    ScreenConstants.STOP_RECORDING -> localViewModel.reset()
                }
            }
        })
    }
}