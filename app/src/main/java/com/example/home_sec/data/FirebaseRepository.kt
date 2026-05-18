package com.example.home_sec.data

import com.google.firebase.database.FirebaseDatabase
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.tasks.await
import kotlinx.coroutines.withContext

class FirebaseRepository {

    private val database by lazy {
        FirebaseDatabase.getInstance().reference
    }

    // ---------------- LOCK ----------------

    suspend fun setLockState(isLocked: Boolean): Boolean = withContext(Dispatchers.IO) {
        try {
            database
                .child("devices")
                .child("doorLock")
                .child("locked")
                .setValue(isLocked)
                .await()

            true
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }

    // ---------------- LED (NEW CLEAN VERSION) ----------------

    suspend fun setLedState(
        power: Boolean,
        mode: Int,
        color: Int,
        brightness: Int
    ): Boolean = withContext(Dispatchers.IO) {
        try {
            val data = mapOf(
                "power" to power,
                "mode" to mode,
                "color" to color,
                "brightness" to brightness
            )

            database
                .child("devices")
                .child("led")
                .setValue(data)
                .await()

            true
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }
    suspend fun setLedField(field: String, value: Any) {
        database.child("devices").child("led").child(field)
            .setValue(value)
            .await()
    }

    // ---------------- DOOR HELPERS ----------------

    suspend fun unlockDoor() = setLockState(false)

    suspend fun lockDoor() = setLockState(true)
}