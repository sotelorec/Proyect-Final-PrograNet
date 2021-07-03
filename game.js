window.onload = function() {

	var game = new Phaser.Game(1980, 1080, Phaser.CANVAS, 'FlappyBird');

	var bird;

	var birdGravity = 800;

	var birdSpeed = 125;

	var birdFlapPower = 300;

	var pipeInterval = 2000;

	var pipeHole = 215;
	var pipeGroup;
	var Socket = new WebSocket('ws://' + '192.168.0.11' + ':81')
  	var play = function(game){};

    play.prototype = {
		preload:function(){
		  
			game.load.spritesheet("bird",'birdSprite.png',45,32,3);
			game.load.image("pipe", "pipe.png");
			//this.load.image("background", 'fondo1.jpg');
			
		},
		create:function(){
			pipeGroup = game.add.group();
			game.stage.backgroundColor = "#000000";
			game.stage.disableVisibilityChange = false;
			game.physics.startSystem(Phaser.Physics.ARCADE);

			bird = game.add.sprite(80,240,"bird");
			bird.animations.add("fly");
			bird.animations.play("fly",6,true);
			game.physics.arcade.enable(bird);
			bird.body.gravity.y = birdGravity;
			//Funcion para hacer volar al pajaro
			game.input.onDown.add(flap, this);

			game.time.events.loop(pipeInterval, addPipe);
			addPipe();
		},
		update:function(){
			game.physics.arcade.collide(bird, pipeGroup, die);
			if(bird.y > game.height){
				die();
			}
		},
		
	};

   game.state.add("Play",play);
   game.state.start("Play");

    Socket.onmessage = function (e) {
		if (parseInt(e.data) < 150) {
			flap();
		}
			
	};

	function flap(){
		bird.body.velocity.y = -birdFlapPower;
	}



	function addPipe(){

		var pipeHolePosition = game.rnd.between(500,430-pipeHole);

		var upperPipe = new Pipe(game,320,pipeHolePosition-480,-birdSpeed);
		game.add.existing(upperPipe);
		pipeGroup.add(upperPipe);
		var lowerPipe = new Pipe(game,320,pipeHolePosition+pipeHole,-birdSpeed);
		game.add.existing(lowerPipe);
		pipeGroup.add(lowerPipe);
		
	}

	function die(){
		game.state.start("Play");
	}



	Pipe = function (game, x, y, speed) {
		Phaser.Sprite.call(this, game, x, y, "pipe");
		game.physics.enable(this, Phaser.Physics.ARCADE);
		this.body.velocity.x = speed;
	};

	Pipe.prototype = Object.create(Phaser.Sprite.prototype);
	Pipe.prototype.constructor = Pipe;

	Pipe.prototype.update = function() {
		if(this.x < -this.width){
			this.destroy();
		}
	};
};