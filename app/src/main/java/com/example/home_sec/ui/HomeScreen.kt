package com.example.home_sec.ui

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Home
import androidx.compose.material3.Button
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.NavigationBar
import androidx.compose.material3.NavigationBarItem
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.background
import androidx.compose.foundation.BorderStroke
import androidx.compose.material.icons.filled.Bolt
import androidx.compose.material3.Slider
import androidx.compose.material.icons.filled.Lock
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.sp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import androidx.compose.material3.SnackbarHost
import androidx.compose.material3.SnackbarHostState
import androidx.compose.runtime.LaunchedEffect




// ---------------- COLOR MAPPING ----------------

fun LedColor.toComposeColor(): Color {
    return when (this) {
        LedColor.RED -> Color.Red
        LedColor.GREEN -> Color.Green
        LedColor.BLUE -> Color.Blue
        LedColor.YELLOW -> Color.Yellow
        LedColor.CYAN -> Color.Cyan
        LedColor.PINK -> Color.Magenta
        LedColor.GRAY -> Color.Gray
        LedColor.NONE -> Color.Transparent
    }
}

@Composable
fun PresetColorPicker(
    viewModel: HomeViewModel = viewModel()
) {
    var selectedColor by remember { mutableStateOf(LedColor.GRAY) }

    val ledState by viewModel.ledState.collectAsStateWithLifecycle()
    val brightness = ledState.brightness
    val power = ledState.power

    Column(horizontalAlignment = Alignment.CenterHorizontally) {

        // Preview (simple placeholder, no RGB math)
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .height(120.dp)
                .background(
                    if (power)
                        selectedColor.toComposeColor().copy(alpha = brightness / 255f)
                    else
                        Color.Black
                )
        )

        Spacer(modifier = Modifier.height(16.dp))

        Row(
            horizontalArrangement = Arrangement.SpaceEvenly,
            modifier = Modifier.fillMaxWidth()
        ) {
            LedColor.entries
                .filter { it != LedColor.NONE }
                .forEach { color ->

                    Button(
                        onClick = {
                            selectedColor = color
                            viewModel.setColor(color)
                        },
                        modifier = Modifier.size(48.dp),
                        contentPadding = PaddingValues(0.dp),
                        border = if (selectedColor == color)
                            BorderStroke(2.dp, Color.White)
                        else null
                    ) {
                        Box(
                            modifier = Modifier
                                .fillMaxSize()
                                .background(color.toComposeColor())
                        )
                    }
                }
        }

        Spacer(modifier = Modifier.height(12.dp))

        Text("Brightness", color = Color.White)

        Slider(
            value = brightness / 200f,
            onValueChange = {
                viewModel.setBrightness(it)
            },
            valueRange = 0f..1f
        )
    }
}
@Composable
fun BottomNavBar(
    selectedIndex: Int = 0,
    onItemSelected: (Int) -> Unit = {}
) {
    val items = listOf(
        NavItem("Home", Icons.Default.Home)
    )

    NavigationBar(containerColor = Color(0xFF1E293B)) {
        items.forEachIndexed { index, item ->
            NavigationBarItem(
                icon = { Icon(item.icon, contentDescription = item.label) },
                label = { Text(item.label) },
                selected = selectedIndex == index,
                onClick = { onItemSelected(index) }
            )
        }
    }
}

data class NavItem(val label: String, val icon: androidx.compose.ui.graphics.vector.ImageVector)


@Composable
fun HomeSecurityScreen(
    viewModel: HomeViewModel = viewModel()
) {
    val isLocked by viewModel.isLocked.collectAsState()
    val ledState by viewModel.ledState.collectAsStateWithLifecycle()
    val power = ledState.power
    val currentMode = ledState.mode
    val snackbarHostState = remember { SnackbarHostState() }
    val error by viewModel.ledError.collectAsState()

    LaunchedEffect(error) {
        error?.let {
            snackbarHostState.showSnackbar(it)
            viewModel.clearLedError()
        }
    }


    Scaffold(
        snackbarHost =  { SnackbarHost(snackbarHostState) },
        bottomBar = { BottomNavBar() },
        containerColor = Color(0xFF121212)
    ) { padding ->

        Column(
            modifier = Modifier
                .padding(padding)
                .padding(16.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {

            Text(
                text = "The Smart Lock",
                color = Color.White,
                fontSize = 30.sp,
                style = MaterialTheme.typography.titleMedium.copy(
                    fontWeight = FontWeight.Bold,
                    letterSpacing = 1.2.sp
                ),
                modifier = Modifier.fillMaxWidth(),
                textAlign = TextAlign.Center
            )

            Spacer(modifier = Modifier.height(12.dp))

            Card(
                colors = CardDefaults.cardColors(containerColor = Color(0xFF1B263B)),
                elevation = CardDefaults.cardElevation(defaultElevation = 6.dp),
                shape = RoundedCornerShape(16.dp),
                modifier = Modifier
                    .fillMaxWidth()
            ) {

                Column(
                    modifier = Modifier
                        .padding(16.dp),
                    horizontalAlignment = Alignment.CenterHorizontally,
                    verticalArrangement = Arrangement.Center
                ) {

                    Text(
                        "Outdoor Lighting Settings",
                        color = Color.White,
                        fontSize = 20.sp,
                        style = MaterialTheme.typography.titleMedium.copy(
                            fontWeight = FontWeight.Bold,
                            letterSpacing = 1.2.sp
                        ),
                        modifier = Modifier.fillMaxWidth(),
                        textAlign = TextAlign.Center
                    )
                    Icon(
                        imageVector = Icons.Default.Bolt,
                        contentDescription = "Bolt Icon",
                        tint = Color.White,
                        modifier = Modifier.size(48.dp)
                    )

                    Spacer(modifier = Modifier.height(16.dp))

                    PresetColorPicker(viewModel)

                    Spacer(modifier = Modifier.height(16.dp))

                    Box(
                        modifier = Modifier.fillMaxWidth(),
                        contentAlignment = Alignment.Center
                    ) {
                        Button(
                            onClick = { viewModel.togglePower() },
                        ) {
                            Text(
                                if (power) "Turn Off"
                                else "Turn On"
                            )
                        }
                    }

                    Spacer(modifier = Modifier.height(12.dp))

                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceEvenly
                    ) {

                        Button(
                            onClick = {
                                viewModel.setMode(LedMode.RAINBOW)
                            },
                            border = if (currentMode == LedMode.RAINBOW)
                                BorderStroke(2.dp, Color.White)
                            else null
                        )
                        { Text("Rainbow") }

                        Button(
                            onClick = {
                                viewModel.setMode(LedMode.CHASER)
                            },
                            border = if (currentMode == LedMode.CHASER)
                                BorderStroke(2.dp, Color.White)
                            else null
                        ) { Text("Chaser") }

                        Button(
                            onClick ={
                                viewModel.setMode(LedMode.BLINK)
                            },
                            border = if (currentMode == LedMode.BLINK)
                                BorderStroke(2.dp, Color.White)
                            else null
                        ) { Text("Blink") }
                    }
                }
            }

                Spacer(modifier = Modifier.height(16.dp))


                Card(
                    colors = CardDefaults.cardColors(containerColor = Color(0xFF1B263B)),
                    elevation = CardDefaults.cardElevation(defaultElevation = 6.dp),
                    shape = RoundedCornerShape(16.dp),
                    modifier = Modifier
                        .fillMaxWidth()
                ) {
                    Column(
                        modifier = Modifier
                            .padding(16.dp)
                            .fillMaxWidth(),
                        horizontalAlignment = Alignment.CenterHorizontally,
                        verticalArrangement = Arrangement.Center
                    ) {

                        Icon(
                            imageVector = Icons.Default.Lock,
                            contentDescription = "Lock Icon",
                            tint = Color.White,
                            modifier = Modifier.size(48.dp)
                        )

                        Spacer(modifier = Modifier.height(12.dp))

                        Button(
                            onClick = {
                                if (isLocked) viewModel.unlockDoor()
                                else viewModel.lockDoor()
                            },
                            modifier = Modifier
                                .width(200.dp)
                                .height(56.dp)
                        ) {
                            Text(
                                    if (isLocked) "Unlock Door"
                                    else "Lock Door"
                                )

                        }
                    }
                }

                Spacer(modifier = Modifier.height(8.dp))

        }
    }
}