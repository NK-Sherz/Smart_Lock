package com.example.home_sec.ui

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.home_sec.data.FirebaseRepository
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.launch
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay


enum class LedMode(val value1:Int) {
    RAINBOW(0),
    CHASER(1),
    BLINK(2),
    STATIC(3)
}

enum class LedColor(val value2:Int) {
    NONE(0),
    GRAY(1),
    PINK(2),
    CYAN(3),
    YELLOW(4),
    BLUE(5),
    GREEN(6),
    RED(7)
}

data class LedState(
    val power: Boolean = true,
    val mode: LedMode = LedMode.RAINBOW,
    val color: LedColor = LedColor.BLUE,
    val brightness: Int = 100
)

class HomeViewModel(
    private val repository: FirebaseRepository = FirebaseRepository()
) : ViewModel() {

    private val _ledState = MutableStateFlow(LedState())
    val ledState: StateFlow<LedState> = _ledState

    private val _ledError = MutableStateFlow<String?>(null)
    val ledError: StateFlow<String?> = _ledError


    private var brightnessJob: Job? = null
    

    // ---------------- CORE UPDATE FUNCTION ----------------

    private fun updateLed(
        power: Boolean = _ledState.value.power,
        mode: LedMode = _ledState.value.mode,
        color: LedColor = _ledState.value.color,
        brightness: Int = _ledState.value.brightness
    ) {
        val previousState = _ledState.value

        val newState = LedState(
            power = power,
            mode = mode,
            color = color,
            brightness = brightness
        )

        _ledState.value = newState   // optimistic update
        _ledError.value = null

        viewModelScope.launch {
            val success = repository.setLedState(
                power = newState.power,
                mode = newState.mode.value1,
                color = newState.color.value2,
                brightness = newState.brightness
            )


            if (!success) {
                _ledState.value = previousState // rollback
                _ledError.value = "Failed to update LED"
            }
        }
    }

    fun clearLedError() {
        _ledError.value = null
    }

    // ---------------- USER ACTIONS ----------------

    fun setMode(mode: LedMode) {
        updateLed(mode = mode)
    }

    fun setColor(color: LedColor) {
        updateLed(
            color = color,
            mode = LedMode.STATIC
        )
    }

    fun setBrightness(value: Float) {
        brightnessJob?.cancel()
        brightnessJob = viewModelScope.launch {
            delay(100)
            val b = (value * 200).toInt().coerceIn(0, 200)
            updateLed(brightness = b)
        }
    }

    fun togglePower() {
        updateLed(power = !_ledState.value.power)
    }

    // ---------------- LOCK (UNCHANGED) ----------------

    private val _isLocked = MutableStateFlow(true)
    val isLocked: StateFlow<Boolean> = _isLocked

    fun unlockDoor() {
        viewModelScope.launch {
            val success = repository.unlockDoor()
            if (success) _isLocked.value = false

        }
    }

    fun lockDoor() {
        viewModelScope.launch {

            val success = repository.lockDoor()
            if (success) _isLocked.value = true
        }
    }
}