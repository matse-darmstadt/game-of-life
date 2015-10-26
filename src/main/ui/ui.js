$(function () {
	var _ = window || this;
	
	var canvas = $('#canvas').get(0);
	canvas.style.cursor = 'crosshair';
	canvas.style.borderColor = '#d62d20';
			
	var container = $('#canvas-container');
	
	canvas.width = container.width() - 8;
	canvas.height = container.height() - 8;
	
	var ctx = canvas.getContext('2d');
	ctx.fillStyle = '#ffa700';
	ctx.font = '18px Consolas';
	
	var gameState = false, tool = 0;
	
	var fieldSize = 20;
	
	var render, renderMethod, renderFrame, refresh = true, possibleRenderMethods = [
		'requestAnimationFrame',
		'mozRequestAnimationFrame',
		'webkitRequestAnimationFrame',
		'oRequestAnimationFrame',
		'msRequestAnimationFrame'
	], renderData = {
		height: (canvas.height / fieldSize) | 0,
		width: (canvas.width / fieldSize) | 0,
		x: [],
		y: []
	};
	
	for (var index = 0, length = possibleRenderMethods.length; index != length; ++index)
		if (_[possibleRenderMethods[index]]) {
			renderMethod = _[possibleRenderMethods[index]];
			break;
		}
		
	if (renderMethod) {
		(render = function () {
			
			if (refresh) {
				refresh = false;
				
				ctx.clearRect(0, 0, canvas.width, canvas.height);
				
				try {
					
					for (var index = 0, length = renderData.x.length; index != length; ++index)
						ctx.fillRect(renderData.x[index] * fieldSize, renderData.y[index] * fieldSize, fieldSize, fieldSize);
					
				} catch (exception) {
					console.log(exception);
					refresh = true;
				}
			}
			
			renderFrame = renderMethod(render);
		})();
		
		var useTool = function (px, py) {
			if (gameState)
				return;
			
			var x = (px / fieldSize) | 0;
			var y = (py / fieldSize) | 0;
			
			var value = ctx.getImageData(px, py, 1, 1).data.join('');
			
			if (tool == 0 && value == '0000') {
				
				renderData.x.push(x);
				renderData.y.push(y);
				
			} else if (tool == 1 && value != '0000') {
				
				for (var index = 0, length = renderData.x.length; index != length; ++index)
					if (renderData.x[index] == x && renderData.y[index] == y) {
						renderData.x.splice(index, 1);
						renderData.y.splice(index, 1);
						break;
					}
					
			}
			
			refresh = true;
		};
		
		$(canvas).click(function (e) {
			try {
				
				useTool(e.clientX - 32, e.clientY - 32);
			
			} catch (exception) {
				console.log(exception);
			}
		});
		
		var play = function () {
			gameState = refresh = true;
			canvas.style.cursor = 'default';
			canvas.style.borderColor = '#008744';
		};
		
		var pause = function () {
			gameState = false;
			refresh = true;
			canvas.style.cursor = 'crosshair';
			canvas.style.borderColor = '#d62d20';
		};
		
		$('#play-button').click(play);
		
		$('#pause-button').click(pause);
		
		$('#populate-button').click(function () {
			pause();
			tool = 0;
		});
		
		$('#erase-button').click(function () {
			pause();
			tool = 1;
		});
	} else
		_.alert('NO RENDER METHOD FOUND');
});
