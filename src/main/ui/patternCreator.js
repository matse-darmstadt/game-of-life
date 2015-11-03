$(function () {
	var _ = window || this;
	
	var canvas = $('#pattern-creator-canvas').get(0);
	canvas.style.cursor = 'crosshair';
	canvas.style.borderColor = '#d62d20';
			
	var panel = $('#pattern-creator');
			
	var container = $('#pattern-creator-canvas-container');
	
	panel.show();
	
	var offset = $(canvas).offset();
	
	canvas.width = container.width();
	canvas.height = container.height();
	
	panel.hide();
	
	var ctx = canvas.getContext('2d');
	ctx.fillStyle = '#ffa700';
	
	var pcTool = 0;
	
	var fieldsPerRow = 21;
	
	var center = 10;
	
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
				
				ctx.strokeStyle = '#fff';
				
				try {
					
					for (var index = 0, length = renderData.populatedFields.length; index != length; ++index)
						ctx.fillRect(renderData.populatedFields[index] * fieldSize, renderData.populatedFields[++index] * fieldSize, fieldSize, fieldSize);
					
					if (mousePos.x != null && mousePos.y != null)
						ctx.strokeRect(mousePos.x * fieldSize, mousePos.y * fieldSize, fieldSize, fieldSize);
					
				} catch (exception) {
					console.log(exception);
					refresh = true;
				}
				
				ctx.strokeStyle = '#f00';
				
				if (!(mousePos.x == center && mousePos.y == center))
					ctx.strokeRect(center * fieldSize, center * fieldSize, fieldSize, fieldSize);
			}
			
			renderFrame = renderMethod(render);
		})();
		
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
			
			var x = (px / fieldSize) | 0;
			var y = (py / fieldSize) | 0;
			
			var exists = fieldExists(px, py);
			
			if (pcTool == 0 && exists == false)
				createField(x, y);
			else if (pcTool == 1 && exists)
				deleteField(x, y);
			
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
		
		$('#pattern-creator-populate-button').click(function () {
			pcTool = 0;
		});
		
		$('#pattern-creator-erase-button').click(function () {
			pcTool = 1;
		});
		
		$('#pattern-creator-clear-button').click(function () {
			renderData.populatedFields = [];
			refresh = true;
		});
		
		$('#pattern-creator-save-button').click(function () {
			var name = prompt('Pattern name:');
			
			var pattern = _.patterns[name] = {
				x: [],
				y: []
			};
			
			for (var index = 0, length = renderData.populatedFields.length; index != length; ++index) {
				pattern.x.push(renderData.populatedFields[index] - center);
				pattern.y.push(renderData.populatedFields[++index] - center);
			}
			
			_.createPattern(name, _.patterns[name]);
			
			var expiration = new Date();
			expiration.setDate(expiration.getDate() + 1);
			
			_.document.cookie = 'patterns=' + _.JSON.stringify(_.patterns) + '; expires=' + expiration.toUTCString();
			
			panel.hide();
		});
		
		$('#pattern-creator-cancel-button').click(function () {
			panel.hide();
		});
		
	} else
		_.alert('NO RENDER METHOD FOUND');
});
	