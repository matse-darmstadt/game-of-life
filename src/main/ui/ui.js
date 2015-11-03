var patterns = {
	Glider: {
		x: [ -1, 1, 0, 1, 0 ],
		y: [ -1, -1, 0, 0, 1 ]
	},
	'r-Pentomino': {
		x: [ 0, 1, -1, 0, 0 ],
		y: [ -1, -1, 0, 0, 1 ]
	},
	Bomb: {
		x: [ -1, 0, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 0, 1 ],
		y: [ -3, -3, -3, -2, -2, -1, -1, 1, 1, 2, 2, 3, 3, 3 ]
	}
};

function deleteAllCookies() {
    var cookies = document.cookie.split(";");

    for (var i = 0; i < cookies.length; i++) {
    	var cookie = cookies[i];
    	var eqPos = cookie.indexOf("=");
    	var name = eqPos > -1 ? cookie.substr(0, eqPos) : cookie;
    	document.cookie = name + "=;expires=Thu, 01 Jan 1970 00:00:00 GMT";
    }
}

$(function () {
	var _ = window || this;

	_.createPattern = function (name, pattern)  {
				
		var option = _.document.createElement('OPTION');
		
		option.setAttribute('value', name);
		option.textContent = name;
		
		$('#spawn-pattern-dropdown').append(option);
		
	};
	
	var cookie = _.document.cookie;
	
	if (cookie)
		_.patterns = _.JSON.parse(cookie.split(';')[0].split('=')[1]);
	
	for (name in _.patterns)
		_.createPattern(name, _.patterns[name]);
	
	var canvas = $('#canvas').get(0);
	canvas.style.cursor = 'crosshair';
	canvas.style.borderColor = '#d62d20';
			
	var container = $('#canvas-container');
	
	var offset = $(canvas).offset();
	
	canvas.width = container.width() - 8;
	canvas.height = container.height() - 8;
	
	var ctx = canvas.getContext('2d');
	ctx.fillStyle = '#ffa700';
	ctx.strokeStyle = '#fff';
	
	var connection;
	
	var gameState = false;
	
	_.tool = 0;
	
	var fieldsPerRow = 50;
	
	var tmp = Number(prompt('How many fields per row do you want?'));
	if (tmp === tmp && tmp > fieldsPerRow)
		fieldsPerRow = tmp;
	
	var fieldSize = _.Math.round(canvas.width / fieldsPerRow);
	
	var mousePos = {
		px: null,
		py: null,
		x: null,
		y: null
	};
	
	var render, renderMethod, renderFrame, refresh = true, possibleRenderMethods = [
		'requestAnimationFrame',
		'mozRequestAnimationFrame',
		'webkitRequestAnimationFrame',
		'oRequestAnimationFrame',
		'msRequestAnimationFrame'
	], renderData = {
		height: _.Math.round(canvas.height / fieldSize),
		width: _.Math.round(canvas.width / fieldSize),
		populatedFields: []
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
					
					for (var index = 0, length = renderData.populatedFields.length; index != length; ++index)
						ctx.fillRect(renderData.populatedFields[index] * fieldSize, renderData.populatedFields[++index] * fieldSize, fieldSize, fieldSize);
					
					if (mousePos.x != null && mousePos.y != null)
						ctx.strokeRect(mousePos.x * fieldSize, mousePos.y * fieldSize, fieldSize, fieldSize);
					
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
			return ctx.getImageData(px, py, 1, 1).data.join('') == '2551670255';
		};
		
		var createField = function (x, y) {
			renderData.populatedFields.push(x);
			renderData.populatedFields.push(y);
		};
		
		var deleteField = function (x, y) {
			for (var index = 0, length = renderData.populatedFields.length; index != length; index += 2)
				if (renderData.populatedFields[index] == x && renderData.populatedFields[index + 1] == y) {
					renderData.populatedFields.splice(index, 2);
					break;
				}	
		};
		
		var useTool = function (px, py) {
			if (gameState)
				return;
			
			var x = (px / fieldSize) | 0;
			var y = (py / fieldSize) | 0;
			
			var exists = fieldExists(px, py);
			
			if (_.tool == 0 && exists == false)
				createField(x, y);
			else if (_.tool == 1 && exists)
				deleteField(x, y);
			else if (_.tool instanceof Object)
				createPattern(px, py, x, y, _.tool);
			
			refresh = true;
		};
		
		$(canvas).click(function (e) {
			try {
				
				mousePos.x = mousePos.y = null;
				
				useTool(e.clientX - offset.left, e.clientY - offset.top);
			
			} catch (exception) {
				console.log(exception);
			}
		}).mousemove(function (e) {
			
			if (gameState)
				return;
			
			mousePos.px = e.clientX - offset.left;
			mousePos.py = e.clientY - offset.top;
			
			if (!fieldExists(mousePos.px, mousePos.py)) {
				mousePos.x = (mousePos.px / fieldSize) | 0;
				mousePos.y = (mousePos.py / fieldSize) | 0;
			} else
				mousePos.x = mousePos.y = null;
			
			refresh = true;
			
		}).mouseleave(function () {
			
			mousePos.x = mousePos.y = null;
			
			refresh = true;
			
		});
		
		var setPauseFrame = function () {
			gameState = false;
			refresh = true;
			canvas.style.cursor = 'crosshair';
			canvas.style.borderColor = '#d62d20';
		};
		
		var play = function () {
			if (!connection) {
				try {
					
					//connection = new _.WebSocket('wss://' + prompt('Hostname:'));
					connection = new _.WebSocket('wss://echo.websocket.org');
					
					connection.onopen = function () {
						connection.send('PLAY');
					};
					
					connection.onclose = function () {
						setPauseFrame();
						connection = null;
						alert('Connection closed!');
					};
					
					connection.onmessage = function (e) {
						if (e.data == 'PLAY') {
							mousePos.x = mousePos.y = null;
							gameState = refresh = true;
							canvas.style.cursor = 'default';
							canvas.style.borderColor = '#008744';
						} else if (e.data == 'PAUSE')
							setPauseFrame();
						else {
							renderData = _.JSON.parse(e.data);
							refresh = true;
						}
					};
					
					connection.onerror = function () {
						_.pause();
						connection = null;
						alert('An error occured and the connection is closed!');
					};
					
				} catch (exception) {
					connection = null;
					alert('An error occured and the connection is closed!');
				}
			} else
				connection.send('PLAY');
		};
		
		_.pause = function () {
			if (connection && gameState)
				connection.send('PAUSE');
		};
		
		$('#play-button').click(play);
		
		$('#pause-button').click(_.pause);
		
		$('#populate-button').click(function () {
			_.pause();
			_.tool = 0;
		});
		
		$('#erase-button').click(function () {
			_.pause();
			_.tool = 1;
		});
		
		$('#clear-button').click(function () {
			_.pause();
			renderData.populatedFields = [];
			refresh = true;
		});
		
		$('#pattern-creator').resize();
		
		$('#create-pattern').click(function () {
			$('#pattern-creator').show();
		});
		
		$('#spawn-pattern').click(function () {
			_.pause();
			_.tool = _.patterns[$('#spawn-pattern-dropdown').val()];
		});
		
	} else
		_.alert('NO RENDER METHOD FOUND');
});
