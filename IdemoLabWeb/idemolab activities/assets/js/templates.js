function box(result){
	var username = result.user.name
		, userimage = result.user.profile_image_url
		, text = result.text;
	
	var box = '<div class="span3 person box"><h2>'+username+'</h2><div class="row-fluid"> <p> <a href="#" class="thumbnail span4"> <img src="'+ userimage +'" alt="">     </a></p>          <p class="span8">'+text+'</p> </div><!--/row-->          <p><a class="btn" href="#">Project x</a><a class="btn" href="#">Project y</a></p> </div><!--/span-->';
	return box;
}

// this is the box of a mention
// therefore I need to 
function person_box(result){
	var username = result.user.name
		, userimage = result.user.profile_image_url;
	
	var box = '<div class="span3 person box"><h2>'+username+'</h2><div class="row-fluid"> <p> <a href="#" class="thumbnail span4"> <img src="'+ userimage +'" alt="">     </a></p>          <p class="span8">'+text+'</p> </div><!--/row-->          <p><a class="btn" href="#">Project x</a><a class="btn" href="#">Project y</a></p> </div><!--/span-->';
	return box;
}


function build_person_box(person){
	var box = '<div class="span3 person box '+person+'"><h2>'+person+'</h2></div><!--/span-->';
	return box;
}

function populate_person_box(result){
	var username = result.user.name
		, userimage = result.user.profile_image_url
		, text = result.text;
 	var box_content = '<div class="row-fluid"> <p> <a href="#" class="thumbnail span4"> <img src="'+ userimage +'" alt="">     </a></p>          <p class="span8 box-content">'+text+'</p> </div><!--/row-->          <p><a class="btn" href="#">Project x</a><a class="btn" href="#">Project y</a></p>';
	return box_content;
}

function addto_person_box(result){
	
}

function get_people_from_results(timeline){
	var people = [];
		
	for(i = 0; i <timeline.length;i++){

		if(timeline[i].in_reply_to_screen_name == null){
 			if (people.indexOf(timeline[i].user.name) == -1){
				people.push(timeline[i].user.name);
			}
			
			if (timeline[i].text.indexOf("@")!= -1){
				var s = timeline[i].text.split(' ');
				// get the name of the person mentioned
				for (var j = 0; j<s.length;j++){
				
					if (s[j].indexOf('@') == 0 && people.indexOf(s[j].split('@')[1])==-1){
						people.push(s[j].split('@')[1]);
					}
				}
			}
		}
		else { 
			if (people.indexOf(timeline[i].in_reply_to_screen_name) == -1){
				people.push(timeline[i].in_reply_to_screen_name);
			}
		}
	}
//	console.log(people);
	return people;
}


function get_results_for_people(people,timeline){
	var people_timeline = {};
	for (i = 0; i<people.length;i++){
		people_timeline[people[i]] = [];
	}
	console.log(people_timeline);
	for (i = 0; i<timeline.length;i++){
		// case 1: no explicit mention detected
		if(timeline[i].in_reply_to_screen_name == null){
			var person = timeline[i].user.name;
			if (typeof people_timeline[person] === 'undefined'){// || people_timeline[person].indexOf(timeline[i].id) == -1){
				//people_timeline.push(person,timeline[i].id);
				people_timeline[person]=[timeline[i].id];
			}else people_timeline[person].push(timeline[i].id);
			
		// case 2: mention hidden in the text
			if (timeline[i].text.indexOf("@")!= -1){
				var s = timeline[i].text.split(' ');
				// get the name of the person mentioned
				for (var j = 0; j<s.length;j++){
					
					if (s[j].indexOf('@') == 0){ // we found a hidden mention :)
						var person = s[j].split('@')[1];
						if (typeof people_timeline[person] === 'undefined'){
							//people_timeline.push(person,timeline[i].id);
							people_timeline[person]=[timeline[i].id];
						}
						else if( people_timeline[person].indexOf(timeline[i].id)==-1){
							//people_timeline.push(person,timeline[i].id);
							people_timeline[person].push(timeline[i].id);					
						}
					
					}
				}
			}
		}
		// case 3: explicit mention
		else { 
			// if there is a mention then I add the tweet id to the mentioned user if it is not already present
			var person = timeline[i].in_reply_to_screen_name;
			if (typeof people_timeline[person] === 'undefined'){
				//people_timeline.push(person,timeline[i].id);
				people_timeline[person]=[timeline[i].id];
			}else
			if (people_timeline[person].indexOf(timeline[i].id) == -1){
				//people_timeline.push(person,timeline[i].id);
				people_timeline[person].push([timeline[i].id]);
			}
		}
		
		//people_timeline.push([people[i],null]);
	}
	console.log(people_timeline);
	return people_timeline;
}


// handling 4 elements rows
function fix_rows(nRows){
		
		var children = $('#boxes-container').children();
//		console.log("children: ")
//		console.log(children);
		$.each(children, function(key, value){
				if (key%nRows ==0){
					console.log($($value).parent);
					$(value).prepend('<div class="row-fluid boxes-row"');
				}
				if (key%nRows == nRows-1){
//					var tmp = $(value).html();
					$(value).append('</div><!-- /row-fluid boxes-row -->');				
				}
			}); // end .each children
			if (children.length%nRows!=0)
				$('#boxes-container').append('</div><!-- /row-fluid boxes-row -->');
} 
