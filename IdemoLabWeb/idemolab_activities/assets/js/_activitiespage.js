function create_activities_page(timeline){
	
	var people = get_people_from_results(timeline);
	var activities_timeline = get_results_for_activities(all_activities,timeline);
	build_activity_boxes(activities_timeline,timeline);
	
}

function get_results_for_activities(activities,results){
	var activities_timeline = new Array();
	$.each($(activities),function (index,value){
		activities_timeline.push({'name':value,'ids':[],'people':[]});
	});

	// parsing the timeline looking for activities
	$.each($(results),function (index,value){
		// parsing the hashtag of each result looking for matching activities
		$.each($(value.entities.hashtags), function(key,element){

			if ($.inArray(element.text,activities)>-1){
				// console.log(element.text);
				// add the value.id to the corresponding activities_timeline element
				// console.log(activities_timeline.filter(function (activity) { 
				// 					return activity.name == element.text; 
				// 					})[0].ids
				// 				);
				activities_timeline.filter(function (activity) { 
									return activity.name == element.text; 
									})[0].ids.push(value.id)
			}
		});
		
		// TODO get person last tweet, check the activity and add the person to the corresponding activity
		// you can use get_results_for_people
		// filter and get the first item on the list
		// parse the hashtags and add person to the corresponding activity box
		
		
	});
	//console.log(activities_timeline);
	return activities_timeline;
}

function build_activity_boxes(objects,results){
	
	var columns = 3;
	$.each(all_activities, function (index,activity){

		if (index%columns==0){
			console.log("index: "+index);						
			$('#boxes-container').append('<div class="row-fluid boxes-row-'+(index/columns)+'">');
		}

		$('#boxes-container .row-fluid.boxes-row-'+Math.floor(index/columns)).append(build_activity_box(activity));	
		
		$('.box.'+activity).append(populate_activity_box(activity,activities_timeline,results);
	});
}
function build_activity_box(activity){
	var box = '<div class="span4 activity box '+activity+'"><h2>'+activity+'</h2></div><!--/span-->';
	return box;
}

function populate_activity_box(activity,activities_timeline,results){
	
	var result_ids = activities_timeline.filter(function (act) { 
						return act.name == activity; 
						})[0].ids;
						
	var activity_results = results.filter(function (res){
			return res.ids ==
	})
			
						
	var username = result.user.name
		, userimage = result.user.profile_image_url
		, text = result.text;
	var image_content = '<p> <a href="#" class="thumbnail span4"> <img src="'+ userimage +'" alt=""></a> '+username+'</p>';
	
	var box = = '<div class="person">'
				+ image_content
				+ '</div><!--/.person-->';
	return box;
	
}