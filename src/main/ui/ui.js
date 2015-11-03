var patterns = {
	glider: {
		x: [ -1, 1, 0, 1, 0 ],
		y: [ -1, -1, 0, 0, 1 ]
	},
	rPentomino: {
		x: [ 0, 1, -1, 0, 0 ],
		y: [ -1, -1, 0, 0, 1 ]
	},
	bomb: {
		x: [ -1, 0, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 0, 1 ],
		y: [ -3, -3, -3, -2, -2, -1, -1, 1, 1, 2, 2, 3, 3, 3 ]
	}
};

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
	
	var connection;
	
	var gameState = false, tool = 0;
	
	var fieldsPerRow = 5;
	
	var tmp = Number(prompt('How many fields per row do you want?'));
	if (tmp === tmp && tmp > fieldsPerRow)
		fieldsPerRow = tmp;
	
	var fieldSize = _.Math.round(canvas.width / fieldsPerRow);
	
	var render, renderMethod, renderFrame, refresh = true, possibleRenderMethods = [
		'requestAnimationFrame',
		'mozRequestAnimationFrame',
		'webkitRequestAnimationFrame',
		'oRequestAnimationFrame',
		'msRequestAnimationFrame'
	], renderData = {
		height: _.Math.round(canvas.height / fieldSize),
		width: _.Math.round(canvas.width / fieldSize),
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
		
		var createPattern = function (px, py, x, y, pattern) {
			var ox, oy;
			for (var index = 0, length = pattern.x.length; index != length; ++index)
				if (fieldExists(px + (ox = pattern.x[index]) * fieldSize, py + (oy = pattern.y[index]) * fieldSize) == false)
					createField(x + ox, y + oy);
		};
		
		var fieldExists = function (px, py) {
			if (px < 0 || py < 0)
				return undefined;
			return ctx.getImageData(px, py, 1, 1).data.join('') != '0000';
		};
		
		var createField = function (x, y) {
			renderData.x.push(x);
			renderData.y.push(y);
		};
		
		var deleteField = function (x, y) {
			for (var index = 0, length = renderData.x.length; index != length; ++index)
				if (renderData.x[index] == x && renderData.y[index] == y) {
					renderData.x.splice(index, 1);
					renderData.y.splice(index, 1);
					break;
				}
		};
		
		var useTool = function (px, py) {
			if (gameState)
				return;
			
			var x = (px / fieldSize) | 0;
			var y = (py / fieldSize) | 0;
			
			var exists = fieldExists(px, py);
			
			if (tool == 0 && exists == false)
				createField(x, y);
			else if (tool == 1 && exists)
				deleteField(x, y);
			else if (tool instanceof Object)
				createPattern(px, py, x, y, tool);
			
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
			if (!connection || connection == null) {
				try {
					
					//connection = new _.WebSocket('ws://' + prompt('Server IP:'));
					connection = new _.WebSocket('ws://localhost:1233');
					connection.onopen = function () {
						//connection.send('PLAY');
						connection.send('PLAY____PLAY____PLAY____PLAY____PLAY____PLAY____PLAY____PLAY____PLAY____PLAY____');
					};
					
					connection.onclose = function () {
						pause();
						connection = null;
						alert('Connection closed!');
					};
					
					connection.onmessage = function (e) {
						if (e.data == 'PLAY') {
							gameState = refresh = true;
							canvas.style.cursor = 'default';
							canvas.style.borderColor = '#008744';
						} else if (e.data == 'PAUSE') {
							pause();
						} else {
							renderData = _.JSON.parse(e.data);
							refresh = true;
						}
					};
					
					connection.onerror = function () {
						pause();
						connection = null;
						alert('An error occured and the connection is closed!');
					};
					
				} catch (exception) {
					connection = null;
					alert('An error occured and the connection is closed!');
				}
			}
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
		
		$('#spawn-glider-button').click(function () {
			pause();
			tool = patterns.glider;
		});
		
		$('#spawn-r-pentomino').click(function () {
			pause();
			tool = patterns.rPentomino;
		});
		
		$('#spawn-bomb').click(function () {
			pause();
			tool = patterns.bomb;
		});
	} else
		_.alert('NO RENDER METHOD FOUND');
});
