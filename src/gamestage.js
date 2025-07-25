function GameStage(canvas, minX, minY, maxX, maxY)
{
    this.minX = minX;
    this.minY = minY;
    this.maxX = maxX;
    this.maxY = maxY;

    this.center = [
        (this.minX + this.maxX) * 0.5,
        (this.minY + this.maxY) * 0.5
    ];

    this.gameMin = [0.0, 0.0];
    this.gameMax = [0.0, 0.0];

    this.gameStageHitbox = new Hitbox(minX, minY, maxX, maxY, this);

    this.gameStageHitbox.onMouseDown = this.mouseDown.bind(this);
    this.gameStageHitbox.onMouseUp = this.mouseUp.bind(this);
    this.gameStageHitbox.onMouseMove = this.mouseMove.bind(this);

    this.hitboxes = [];

    this.canvas = canvas;
    var aspect = canvas.width / canvas.height;

    var view = this.view = [1.23, 2.34];
    this.zoomVelocity = 0;
    this.zoomLevel = -82;
    this.updateZoom();

    var size = this.size;
    this.feather = { value: 0.0 };
    this.camera = new THREE.OrthographicCamera(1, 1, 1, 1, 0, 100);
    this.camera.position.z = 100;
    this.invProjectionMatrix = new THREE.Matrix4();
    this.updateCamera();

    this.mousePos = [(this.maxX - this.minX) * 0.5,
                     (this.maxX - this.minX) * 0.5];

    this.onMouseDown = null;
    this.onMouseUp = null;
    this.onMouseMove = null;

    this.debugClipping = false;
}

GameStage.prototype.setView = function setView(x, y, zoom)
{
    this.view = [x, y];
    this.zoomLevel = zoom;
    this.updateZoom();
}

GameStage.prototype.updateCamera = function updateCamera()
{
    var canvas = this.canvas;
    var aspect = canvas.width / canvas.height;

    var view = this.view;
    var size = this.size;
    var camera = this.camera;
    camera.left   = view[0] - size * aspect;
    camera.right  = view[0] + size * aspect;
    camera.top    = view[1] - size;
    camera.bottom = view[1] + size;

    camera.updateProjectionMatrix();
    this.invProjectionMatrix.getInverse(camera.projectionMatrix);

    this.feather.value = Math.max((camera.right - camera.left) / canvas.width, (camera.bottom - camera.top) / canvas.height) * 2.0;
};

GameStage.prototype.updateZoom = function updateZoom()
{
    var zoom = Math.pow(1.05, this.zoomLevel);
    this.zoom = zoom;
    this.size = 1 / zoom;
};

GameStage.prototype.scroll = function scroll(delta)
{
    this.view[0] += delta[0];
    this.view[1] += delta[1];
};

GameStage.prototype.update = function update(deltaTime, panDir, zoomDir)
{
    this.zoomVelocity *= 0.8;
    if (Math.abs(this.zoomVelocity) < 0.001)
    {
        this.zoomVelocity = 0.0;
    }

    if (zoomDir)
    {
        var speed = 20 * deltaTime;
        this.zoomLevel += speed * zoomDir;
        this.updateZoom();
    }

    var oldZoom = this.zoom;
    this.zoomLevel += this.zoomVelocity;
    var maxZoom = 50;
    var minZoom = -150;
    if (this.zoomLevel > maxZoom)
    {
        this.zoomLevel = maxZoom;
        this.zoomVelocity = 0;
    }
    if (this.zoomLevel < minZoom)
    {
        this.zoomLevel = minZoom;
        this.zoomVelocity = 0;
    }
    this.updateZoom();

    var gameMouse = this.toView(this.mousePos);
    var panelCenter = this.toView(this.center);

    var view = this.view;

    // keep whatever is under the mouse stationary during the zoom
    var lerp = oldZoom / this.zoom;
    view[0] = view[0] * lerp + gameMouse[0] * (1 - lerp);
    view[1] = view[1] * lerp + gameMouse[1] * (1 - lerp);

    // move the view slightly so that whatever is under the mouse moves to the center of the game space
    var centerVelocity = Math.abs(this.zoomVelocity);
    var speed = centerVelocity * (this.zoomVelocity < 0 ? 0.1 : 0.03);
    view[0] += (gameMouse[0] - panelCenter[0]) * speed;
    view[1] += (gameMouse[1] - panelCenter[1]) * speed;

    if (panDir)
    {
        var speed = 200 * deltaTime;
        var centerPan = this.toView([this.center[0] + speed * panDir[0], this.center[1] + speed * panDir[1]]);
        view[0] += centerPan[0] - panelCenter[0];
        view[1] += centerPan[1] - panelCenter[1];
    }

    if (this.debugClipping)
    {
        this.gameMin = this.toView([this.minX + 50, this.minY + 50]);
        this.gameMax = this.toView([this.maxX - 50, this.maxY - 50]);
    }
    else
    {
        this.gameMin = this.toView([this.minX, this.minY]);
        this.gameMax = this.toView([this.maxX, this.maxY]);
    }

    this.updateCamera();
};

GameStage.prototype.clearHitboxes = function clearHitboxes()
{
    this.hitboxes.length = 0;
};

GameStage.prototype.zoomDelta = function zoomDelta(deltaY)
{
    this.zoomVelocity += deltaY * 0.005;
};

GameStage.prototype.addHitbox = function addHitbox(hitbox)
{
    if (!hitbox)
    {
        throw new Error("Invalid hitbox");
    }
    this.hitboxes.push(hitbox);
};

GameStage.prototype.removeHitbox = function removeHitbox(hitbox)
{
    if (!hitbox)
    {
        throw new Error("Invalid hitbox");
    }
    var i;
    for (i = 0; i < this.hitboxes.length; i += 1)
    {
        if (this.hitboxes[i] === hitbox)
        {
            this.hitboxes.splice(i, 1);
            return true;
        }
    }
    return false;
};

GameStage.prototype.findHitbox = function findHitbox(x, y)
{
    var i;
    for (i = 0; i < this.hitboxes.length; i += 1)
    {
        var hitbox = this.hitboxes[i];
        if (x > hitbox.minX &&
            x < hitbox.maxX &&
            y > hitbox.minY &&
            y < hitbox.maxY)
        {
            return hitbox;
        }
    }
    return null;
};

GameStage.prototype.mouseDown = function mouseDown(p, button)
{
    this.mousePos = [p[0], p[1]];
    var q = this.toView(p);
    var hitbox = this.findHitbox(q[0], q[1]);
    if (hitbox && hitbox.onMouseDown)
    {
        hitbox.onMouseDown(p, button);
    }
    else if (this.onMouseDown)
    {
        this.onMouseDown(p, button);
    }
};

GameStage.prototype.mouseUp = function mouseUp(p, button)
{
    this.mousePos = [p[0], p[1]];
    var q = this.toView(p);
    var hitbox = this.findHitbox(q[0], q[1]);
    if (hitbox && hitbox.onMouseUp)
    {
        hitbox.onMouseUp(p, button);
    }
    else if (this.onMouseUp)
    {
        this.onMouseUp(p, button);
    }
};

GameStage.prototype.mouseMove = function mouseMove(p)
{
    this.mousePos = [p[0], p[1]];
    var q = this.toView(p);
    var hitbox = this.findHitbox(q[0], q[1]);
    if (hitbox && hitbox.onMouseMove)
    {
        hitbox.onMouseMove(p);
    }
    else if (this.onMouseMove)
    {
        this.onMouseMove(p);
    }
};

GameStage.prototype.transformContext = function transformContext(ctx)
{
    var z = this.zoom;
    ctx.transform(z, 0, 0, z, -this.view[0], -this.view[1]);
};

GameStage.prototype.toView = function toView(p)
{
    var x = (p[0] / this.canvas.clientWidth) * 2 - 1;
    var y = -((p[1] / this.canvas.clientHeight) * 2 - 1);

    var v3 = new THREE.Vector3(x, y, 0.0);
    v3 = v3.applyMatrix4(this.invProjectionMatrix);
    return [v3.x, v3.y];
};

GameStage.prototype.fromView = function fromView(p)
{
    var x = p[0];
    var y = p[1];

    var v3 = new THREE.Vector3(x, y, 0.0);
    v3 = v3.applyMatrix4(this.camera.projectionMatrix);

    x = (x + 1) * 0.5 * this.canvas.clientWidth;
    y = -(y + 1) * 0.5 * this.canvas.clientHeight;

    return [x, y];
};

GameStage.prototype.hitboxOverlaps = function hitboxOverlaps(hitbox)
{
    return (hitbox.maxX >= this.gameMin[0] && this.gameMax[0] >= hitbox.minX &&
            hitbox.maxY >= this.gameMin[1] && this.gameMax[1] >= hitbox.minY);
};

GameStage.prototype.boxOverlaps = function boxOverlaps(x0, y0, x1, y1, padding)
{
    var minX = Math.min(x0, x1) - padding;
    var minY = Math.min(y0, y1) - padding;
    var maxX = Math.max(x0, x1) + padding;
    var maxY = Math.max(y0, y1) + padding;
    return (maxX >= this.gameMin[0] && this.gameMax[0] >= minX &&
            maxY >= this.gameMin[1] && this.gameMax[1] >= minY);
};

GameStage.prototype.drawBorder = function drawBorder(ctx)
{
    var padding = 1;
    if (this.debugClipping)
    {
        padding -= 50;
    }
    var x0 = this.minX - padding;
    var y0 = this.minY - padding;
    var x1 = this.maxX + padding;
    var y1 = this.maxY + padding;

    ctx.beginPath();
    ctx.lineWidth = 1;
    ctx.strokeStyle = "#000000";
    ctx.moveTo(x0, y0);
    ctx.lineTo(x0, y1);
    ctx.lineTo(x1, y1);
    ctx.lineTo(x1, y0);
    ctx.lineTo(x0, y0);
    ctx.stroke();
};

GameStage.prototype.drawHitboxes = function drawHitboxes(ctx)
{
    var i;
    for (i = 0; i < this.hitboxes.length; i += 1)
    {
        var hitbox = this.hitboxes[i];
        ctx.beginPath();
        ctx.lineWidth = 1 * this.size;
        ctx.strokeStyle = "#0000FF";
        ctx.moveTo(hitbox.minX, hitbox.minY);
        ctx.lineTo(hitbox.minX, hitbox.maxY);
        ctx.lineTo(hitbox.maxX, hitbox.maxY);
        ctx.lineTo(hitbox.maxX, hitbox.minY);
        ctx.lineTo(hitbox.minX, hitbox.minY);
        ctx.stroke();
    }
};

GameStage.prototype.setScissor = function setScissor(renderer)
{
    renderer.setScissor(this.minX, this.minY, this.maxX - this.minX, this.maxY - this.minY);
    renderer.setScissorTest(true);
};
