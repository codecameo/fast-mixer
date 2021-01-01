package com.bluehub.fastmixer.screens.mixing

import android.annotation.SuppressLint
import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import com.bluehub.fastmixer.databinding.ListItemAudioFileBinding
import kotlinx.coroutines.Deferred
import kotlinx.coroutines.Job

class AudioFileListAdapter(
        context: Context,
        private val audioFileEventListeners: AudioFileEventListeners,
        private val audioViewSampleCountStore: AudioViewSampleCountStore,
        audioFileList: MutableList<AudioFile>
): ArrayAdapter<AudioFile>(context, -1, audioFileList) {
    @SuppressLint("ViewHolder")
    override fun getView(position: Int, convertView: View?, parent: ViewGroup): View {
        val inflater = context.getSystemService(Context.LAYOUT_INFLATER_SERVICE) as LayoutInflater
        val binding = ListItemAudioFileBinding.inflate(inflater, parent, false)
        binding.audioFile = getItem(position)
        binding.eventListener = audioFileEventListeners
        binding.audioViewSampleCountStore = audioViewSampleCountStore
        return binding.root
    }
}

class AudioFileEventListeners(
    var loadFileCallback: (String) -> Job,
    var readSamplesCallback: (String) -> (Int) -> Deferred<Array<Float>>,
    var deleteFileCallback: (String) -> Unit,
    var getTotalSamples: (String) -> Int
) {
    fun readSamplesCallbackWithIndex(filePath: String): (Int)->Deferred<Array<Float>> = readSamplesCallback(filePath)
    fun loadFileCallbackWithIndex(filePath: String): (Unit) -> Job = { loadFileCallback(filePath) }
    fun getTotalSamplesWithIndex(filePath: String): (Unit) -> Int = { getTotalSamples(filePath) }
}