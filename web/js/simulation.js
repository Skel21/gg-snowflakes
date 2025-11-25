let isPaused = false;
const canvas = $("#simulation");
const playPauseButton = $("#play-pause-button");
const alphaInput = $("#alpha");
const alphaOutput = $("#alpha-output");
const betaInput = $("#beta");
const betaOutput = $("#beta-output");
const muInput = $("#mu");
const muOutput = $("#mu-output");
const kappaInput = $("#kappa");
const kappaOutput = $("#kappa-output");
const rhoInput = $("#rho");
const rhoOutput = $("#rho-output");
const thetaInput = $("#theta");
const thetaOutput = $("#theta-output");
const gridSizeInput = $("#grid-size");
const gridSizeOutput = $("#grid-size-output");
const iterationsPerFrameInput = $("#iterations-per-frame");
const iterationsPerFrameOutput = $("#iterations-per-frame-output");

function reset() {
    Module.reset();
}

function set_alpha(alpha) {
    alphaOutput.text(alpha);
    Module.set_alpha(parseFloat(alpha));
} 

function set_beta(beta) {
    betaOutput.text(beta);
    Module.set_beta(parseFloat(beta));
}

function set_mu(mu) {
    muOutput.text(mu);
    Module.set_mu(parseFloat(mu));
}

function set_kappa(kappa) {
    kappaOutput.text(kappa);
    Module.set_kappa(parseFloat(kappa));
}

function set_rho(rho) {
    rhoOutput.text(rho);
    Module.set_rho(parseFloat(rho));
}

function set_theta(theta) {
    thetaOutput.text(theta);
    Module.set_theta(parseFloat(theta));
}

function play_pause() {
    isPaused = !isPaused;
    playPauseButton.text(isPaused ? "play_arrow" : "pause");
    Module.play_pause(isPaused);
}


function set_iterations_per_frame(iterations) {
    iterationsPerFrameOutput.text(iterations);
    Module.set_iterations_per_frame(parseInt(iterations));
}

function set_grid_size(size) {
    gridSizeOutput.text(size);
    Module.set_grid_size(parseInt(size));
}

function applyPreset(index) {
    Module.apply_preset(index);
    
    // Update all UI controls with actual values from C++
    alphaInput.value = Module.get_current_alpha().toFixed(3);
    alphaInput.val(Module.get_current_alpha().toFixed(3));
    alphaOutput.text(Module.get_current_alpha().toFixed(3));
    
    betaInput.value = Module.get_current_beta().toFixed(2);
    betaInput.val(Module.get_current_beta().toFixed(2));
    betaOutput.text(Module.get_current_beta().toFixed(2));
    
    muInput.value = Module.get_current_mu().toFixed(4);
    muInput.val(Module.get_current_mu().toFixed(4));
    muOutput.text(Module.get_current_mu().toFixed(4));
    
    kappaInput.value = Module.get_current_kappa().toFixed(4);
    kappaInput.val(Module.get_current_kappa().toFixed(4));
    kappaOutput.text(Module.get_current_kappa().toFixed(4));
    
    rhoInput.value = Module.get_current_rho().toFixed(3);
    rhoInput.val(Module.get_current_rho().toFixed(3));
    rhoOutput.text(Module.get_current_rho().toFixed(3));
    
    thetaInput.value = Module.get_current_theta().toFixed(4);
    thetaInput.val(Module.get_current_theta().toFixed(4));
    thetaOutput.text(Module.get_current_theta().toFixed(4));
    
    gridSizeInput.value = Module.get_current_grid_size();
    gridSizeInput.val(Module.get_current_grid_size());
    gridSizeOutput.text(Module.get_current_grid_size());
}

function populatePresets() {
    const presetCount = Module.get_preset_count();
    const presetContainer = $("#preset-container");
    
    let html = '<select id="preset-select" class="w-full bg-gray-700 text-white p-3 rounded-lg">';
    html += '<option value="-1">-- Select a Preset --</option>';
    
    for (let i = 0; i < presetCount; i++) {
        const info = Module.get_preset_info(i);
        html += `<option value="${i}">${info}</option>`;
    }
    
    html += '</select>';
    html += '<div id="preset-description" class="text-sm text-gray-400 mt-2"></div>';
    
    presetContainer.html(html);
    
    $("#preset-select").on("change", function() {
        const index = parseInt($(this).val());
        if (index >= 0) {
            const info = Module.get_preset_info(index);
            $("#preset-description").text(info.description);
            applyPreset(index);
        }
    });
}

Module.onRuntimeInitialized = () => {
    Module.init();

    populatePresets();

    alphaInput.value = "0.4";
    alphaInput.val("0.4");
    set_alpha("0.4");

    betaInput.value = "1.6";
    betaInput.val( "1.6" );
    set_beta( "1.6" );

    muInput.value = "0.015";
    muInput.val( "0.015" );
    set_mu( "0.015" );

    kappaInput.value = "0.005";
    kappaInput.val( "0.005" );
    set_kappa( "0.005" );

    rhoInput.value = "0.635";
    rhoInput.val( "0.635" );
    set_rho( "0.635" );

    thetaInput.value = "0.025";
    thetaInput.val( "0.025" );
    set_theta( "0.025" );

    iterationsPerFrameInput.value = "1";
    iterationsPerFrameInput.val( "1" );
    set_iterations_per_frame( "1" );

    gridSizeInput.value = "400";
    gridSizeInput.val( "400" );
    set_grid_size( "400" );

    if (canvas) {
        let width = canvas.width();
        Module.set_window_size(width);
    }

    Module.set_main_loop();
    console.log("WASM Module Loaded");    
}
