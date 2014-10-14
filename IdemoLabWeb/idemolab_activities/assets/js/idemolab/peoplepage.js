/**
 * this is the js file containing the instruction for building the template of people's page
 *
 * PEOPLE (PERSON) is the main entity on display in this view
 *
 */
function create_people_page(timeline){
	// console.log(timeline); 
	/* PEOPLE can be both static (given array) 
	* or dynamic (retrieved from the available tweets) */ 
	var people = get_people_from_results(timeline);

	//console.log("people: "+people.length + " " +people);

 /* PEOPLE - TIMELINE * organize for each person the tweets related to them 
  * TODO: now it displays only the last tweet emitted. 
  * I would like a carousel of todays status updates */ 
	var people_timelines = get_results_for_people(people,timeline);

 /* 
  * Initialize the boxes for each person 
  */ 
	var lasttweetindex = null; 

	$.each(people, function (index,person){

		if (index%4==0){
		//	console.log("index: "+index);						
			$('#boxes-container').append('<div class="row-fluid boxes-row-'+(index/4)+'">');
		}

		$('#boxes-container .row-fluid.boxes-row-'+Math.floor(index/4)).append(build_person_box(person) );	
		//latesttweetindex = get_latest_tweet_per_person(person,people_timeline);
		var lasttweet = timeline.filter(function (item){
			return item.id == people_timelines[person][0];
		})[0];
		//console.log(lasttweet);
		//$('.box.'+person).append(populate_person_box(timeline[lasttweetindex]));
		$('.box.'+person).append(populate_person_box(person,lasttweet));	
	});
}

function get_latest_tweet_per_person(person, people_timeline, timeline){
	$.each(timeline, function(key,element) {	
		if (element.id == people_timelines[person][0]){
			return key;
		}	 
	}); // end .each
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
	var box = '<div class="span3 person box '+person+' well"><h2>'+person+'</h2></div><!--/span-->';
	return box;
}

function populate_person_box(person,result){
	var username = result.user.name
		//, userimage = result.user.profile_image_url
		, userimage = "http://api.twitter.com/1/users/profile_image/"+person+".jpg"
		, text = result.text;
	var userimage = "../assets/img/userimage.png";	
	var image_content = '<p> <a href="#" class="thumbnail span4"> <img src="'+ userimage +'" alt=""></a></p>';
	
	var act_messages = ''
		, prj_messages = '';

	var text_content = '<p class="box-content">'+text+'</p>';
 	
	
	
	$.each(result.entities.hashtags,function(key,value){
		var activity = value.text
			, type = 'success'
			, type_p = 'success';
			
		var type = get_activity_type(activity);
		act_messages += '<span class="btn btn-small btn-'+type+'">'+activity+'</span>';
		
		if (info_projects.indexOf(activity)>=0){
			type_p = "info";
			prj_messages +='<span class="btn btn-small btn-'+type_p+'">'+activity+'</span>';
		}
		
		
	});
	
	// box_content +=('<div class="row-fluid activities">'+act_messages+'</div><!--/row-->'); 
	// 	box_content +=('<div class="row-fluid projects">'+prj_messages+'</div><!--/row-->'); 
	
	
	var box_content = '<div class="row-fluid person_details"> '
				+ image_content
				+ act_messages
				 
				+ '</div><!--/row-->'
				+ '<div class="row-fluid status_details"> ' 
				+ text_content
				+ prj_messages
				+'</div><!--/row-->';


	return box_content;
}

function update_people_boxes(result){
	console.log(result);
	var people = get_people_from_results([result]);
	$.each(people,function (key,element){

//		$('.box.'+element+ ' p.box-content').html("UPDATE "+data.text+"</br>");
		$('.box.'+element+' .person_details').remove();
		$('.box.'+element+' .status_details').remove();
		$('.box.'+element).append(populate_person_box(element,result));

		// animation: updated elements will blink for 3 times	
		$('#boxes-container .'+element).blink({"maxBlinks":3});
	});
}